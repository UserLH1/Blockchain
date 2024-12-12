var M: number = 12;
var exponent: number = 65537;
var p: number = 17;
var q: number = 23;
var n: number = p * q;

function encryption(base, exp, mod) {
    let result = 1;
    base = base % mod;
  
    while (exp > 0) {
      if (exp % 2 === 1) {
        result = (result * base) % mod;
      }
      exp = Math.floor(exp / 2);
      base = (base * base) % mod;
    }
    
    return result;
  }
console.log(encryption(M,exponent,n));