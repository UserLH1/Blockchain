"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var crypto = require("crypto");
function createHash(firstName, lastName) {
    var word = firstName + "." + lastName + "." + 'SHA256("Blockchain")';
    console.log(word);
    var hash = crypto.createHash('sha256').update(firstName + "." + lastName).digest('hex');
    return hash;
}
var firstName = 'Horatiu';
var lastName = 'Lazea';
console.log(createHash(firstName, lastName));
