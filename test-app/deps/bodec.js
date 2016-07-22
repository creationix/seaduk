/*jshint node:true*/
"use strict";

exports.create = create;
exports.concat = concat;
exports.slice = slice;
exports.indexOf = indexOf;
exports.toString = toString;

function create(buffer) {
  return Buffer(buffer);
}

function concat(base, buffer) {
  return base ? Buffer.concat(base, buffer) : Buffer(buffer);
}

function slice(buffer, start, end) {
  return buffer.slice(start, end);
}

function indexOf(buffer, raw, start) {
  var bl = buffer.length,
      rl = raw.length;
  outer: for (var i = start || 0; i < bl; i++) {
    for (var j = 0; j < rl; j++) {
      if (i + j >= bl || buffer[i + j] !== raw.charCodeAt(j)) {
        continue outer;
      }
    }
    return i;
  }
  return -1;
}

function toString(buffer, start, end) {
  return buffer.slice(start, end).toString();
}
