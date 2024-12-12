var string = 'KRUDWLX H LXELVHD PHD';

for (var shift = 0; shift < 50; shift++) {
  var decryptedString = '';

  for (var i = 0; i < string.length; i++) {
    var char = string[i];

    // Verificăm dacă este o literă
    if (char.match(/[A-Z]/)) {
      var asciiCode = char.charCodeAt(0);
      
      // Aplicăm deplasamentul, asigurându-ne că rămânem în intervalul alfabetului
      var newAsciiCode = ((asciiCode - 65 - shift + 26) % 26) + 65;
      
      var new_char = String.fromCharCode(newAsciiCode);
      decryptedString += new_char;
    } else {
      // Dacă nu e literă (ex: spațiu), păstrăm caracterul
      decryptedString += char;
    }
  }

  // Afișăm textul decriptat pentru deplasamentul curent
  console.log(`Depasament: ${shift}: ${decryptedString}`);
}
