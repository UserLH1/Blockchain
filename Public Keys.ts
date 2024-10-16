var M: number = 12;
var exponent: number = 65537;
var p: number = 17;
var q: number = 23;
var n: number = p * q;

function encryption(base, exp, mod) {
    let result = 1;
    base = base % mod;
  
    while (exp > 0) {
      // Dacă exponentul este impar, înmulțim cu baza
      if (exp % 2 === 1) {
        result = (result * base) % mod;
      }
      // Împărțim exponentul la 2
      exp = Math.floor(exp / 2);
      // Ridicăm baza la pătrat
      base = (base * base) % mod;
    }
    
    return result;
  }
console.log(encryption(M,exponent,n));