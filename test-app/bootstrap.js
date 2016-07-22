/*global nucleus,Duktape,p:true*/

// Register uv as a require-able module.
Duktape.modLoaded.uv = {exports:nucleus.uv};
// Bootstrap require by reusing Duktape's default behavior
// It's mostly node.js like.
Duktape.modSearch = function (id) {
  "use strict";
  var filename = id + ".js";
  var js = nucleus.readfile(filename);
  if (typeof js !== "string") {
    throw new Error("No such file in bundle: " + filename);
  }
  return js;
};

p = nucleus.dofile("deps/utils.js").prettyPrint;
