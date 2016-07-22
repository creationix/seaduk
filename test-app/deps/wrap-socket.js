/*jshint node:true*/
"use strict";

module.exports = wrapSocket;

// socket implements uv_stream_t interface
// decode takes raw data and outputs [value, extra] or nothing
// encode takes value and ouputs raw data
// returns {read,write,socket}
//   read(callback) -> value or undefined
//     read.update(newDecode) update decoder
//   write(value, callback) -> value can be undefined to send EOS
//     write.update(newEncode) update encoder
function wrapSocket(socket, decode, encode) {
  var readBuffer; // A buffer of past read data that's not consumed by the
                  // parser yet.
  // fifo queue where writers can get ahead of readers and vice-verse
  // if reader is higher, then we have multiple reads waiting for data.
  // if writer is higher, we have pending data waiting for readers.
  var reader = 0, writer = 0;
  var queue = {};

  var inEnd = false, outEnd = false, closed = false;

  // Atomic pausing and unpausing of the read stream for
  // proper back-pressure to the socket.
  var paused = true;
  function pause() {
    if (closed || paused) return;
    paused = true;
    socket.readStop();
  }
  function unpause() {
    if (closed || !paused) return;
    paused = false;
    process();
    // Once we're sure process didn't re-pause the stream, startup the source.
    if (!paused) {
      socket.readStart(onRead);
    }
  }

  function onRead(err, chunk) {
    // p("onRaw", err, chunk);
    // If there is data and a decoder, let's process it.
    // TODO: This is probably bad design, we should consider carefully.
    //       Maybe modify the decoder interface to know about EOS events too.
    if (!err && chunk && decode) {
      // Feed the data to the decoder
      readBuffer = decode.concat(readBuffer, chunk);
      return process();
    }
    if (!chunk) {
      inEnd = true;
      checkClose();
    }

    // Otherwise forward everything  directly to onData
    return onData(err, chunk);
  }

  function process() {
    while (readBuffer !== undefined) {
      // Run the data through the decoder, catching any parse errors.
      var out;
      try { out = decode(readBuffer); }
      catch (err) { return onData(err); }

      // If the decoder wants more data, we're done here. Wait for it.
      if (!out) break;

      // Remember the leftover data for next round
      readBuffer = out[1];

      // And emit the event.
      // If it had to buffer the data, let's stop processing.
      if (onData(null, out[0])) break;
    }
  }

  function onData(err, data) {
    // p("onData", err, data);
    // If there is a waiting reader, give it the data.
    if (reader > writer) {
      var callback = queue[writer++];
      callback(err, data);
      return false;
    }
    // If there was an error and no waiting reader, throw it for now.
    // This ensures no swallowed/ignored errors.
    if (err) throw err;

    // Enqueue the data for later readers.
    queue[writer++] = data;
    // Since we had to buffer the data, pause the input.
    pause();
    // Return true to tell the processor to pause as well.
    return true;
  }

  function read(callback) {
    // If there is pending data ready, use it immedietly.
    if (writer > reader) {
      var data = queue[reader++];
      return callback(null, data);
    }
    // Otherwise queue up the reader and unpause the stream.
    queue[reader++] = callback;
    return unpause();
  }

  var done = false;
  function write(data, callback) {
    if (closed) {
      throw new Error("Can't write to already closed socket");
    }
    if (done) {
      throw new Error("Can't write to already shutdown socket");
    }
    if (data && encode) data = encode(data);

    // Normalize data to buffer or null
    if (data || typeof data === 'string') {
      return socket.write(data, callback);
    }
    socket.shutdown(function (err) {
      if (callback) callback(err);
      outEnd = true;
      checkClose();
    });
  }

  function checkClose() {
    if (inEnd && outEnd) {
      closed = true;
      socket.close();
    }
  }


  read.update = function (newDecode) {
    decode = newDecode;
  };
  write.update = function (newEncode) {
    encode = newEncode;
  };

  return {
    read: read,
    write: write,
    socket: socket
  };
}
