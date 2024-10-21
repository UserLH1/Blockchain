import { createHash } from 'crypto';

function generateSHA256Hash(firstName: string, lastName: string): string {
  const blockchainHash = createHash('sha256').update('Blockchain').digest('hex');

  const input = `${firstName}.${lastName}.${blockchainHash}`;

  const finalHash = createHash('sha256').update(input).digest('hex');

  return finalHash;
}

const firstName = "Horatiu";
const lastName = "Lazea"; 

const result = generateSHA256Hash(firstName, lastName);

console.log("Hash-ul generat este:", result);
