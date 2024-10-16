const a = 497;
const b = 1768;
const p = 9739;

const O = null;

type Point = [number, number] | null;

function modInverse(k: number, p: number): number {
  if (k === 0) throw new Error('Cannot compute modular inverse of 0');
  let [s, old_s] = [0, 1];
  let [t, old_t] = [1, 0];
  let [r, old_r] = [p, k];

  while (r !== 0) {
    const quotient = Math.floor(old_r / r);
    [old_r, r] = [r, old_r - quotient * r];
    [old_s, s] = [s, old_s - quotient * s];
    [old_t, t] = [t, old_t - quotient * t];
  }

  const gcd = old_r;
  const x = old_s;

  if (gcd !== 1) throw new Error('Inverse does not exist');
  else return (x + p) % p;
}

function mod(n: number, p: number): number {
  return ((n % p) + p) % p;
}

function pointAdd(P: Point, Q: Point): Point {
  if (P === O) return Q;
  if (Q === O) return P;

  const [x1, y1] = P;
  const [x2, y2] = Q;

  if (x1 === x2 && mod(y1 + y2, p) === 0) return O;

  let lambda: number;

  if (x1 === x2 && y1 === y2) {
    const numerator = mod(3 * x1 * x1 + a, p);
    const denominator = modInverse(2 * y1, p);
    lambda = mod(numerator * denominator, p);
  } else {
    const numerator = mod(y2 - y1, p);
    const denominator = modInverse(x2 - x1, p);
    lambda = mod(numerator * denominator, p);
  }

  const x3 = mod(lambda * lambda - x1 - x2, p);
  const y3 = mod(lambda * (x1 - x3) - y1, p);

  return [x3, y3];
}

function isOnCurve(point: Point): boolean {
  if (point === O) return true;
  const [x, y] = point;
  return mod(y * y, p) === mod(x * x * x + a * x + b, p);
}

const P: Point = [493, 5564];
const Q: Point = [1539, 4742];
const R: Point = [4403, 5202];

const P_plus_P = pointAdd(P, P);
const temp = pointAdd(P_plus_P, Q);
const S = pointAdd(temp, R);

console.log('Point S = P + P + Q + R:');
if (S !== O) {
  const [xS, yS] = S;
  console.log(`S = (${xS}, ${yS})`);
} else {
  console.log('S is the point at infinity O.');
}

if (isOnCurve(S)) {
  console.log('Point S lies on the elliptic curve E.');
} else {
  console.log('Point S does NOT lie on the elliptic curve E.');
}
