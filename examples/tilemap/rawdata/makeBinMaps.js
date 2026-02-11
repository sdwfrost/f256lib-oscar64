
var over = require('./u5world');
var under = require('./u5underworld');

var fs = require('fs');

function writeMap(name, data) {
	var out = fs.createWriteStream(name);
	var buffer = Buffer.from(data[0]);
	var i;
	for (i=1; i<256; i++) {
		buffer = Buffer.concat([ buffer, Uint8Array.from(data[i]) ]);
	}
	out.write(buffer);
	out.end();
}

writeMap('brita.map', over.map());
writeMap('under.map', under.map());
