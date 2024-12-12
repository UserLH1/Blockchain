const fs = require('node:fs');
import * as crypto from 'crypto';
import { createReadStream } from 'fs';
import * as readline from 'readline';

function createHash(password: string): string
{
    const hash:string = crypto.createHash('sha256').update(password).digest('hex');
    return hash;
}

const fileStream = createReadStream('rockyou.txt');

async function breakHash(hash:string): Promise<void>{
  const fileStream = createReadStream('rockyou.txt');

  const rl = readline.createInterface({
    input: fileStream,
    crlfDelay: Infinity
  });

  for await (const line of rl) {
    const password = line.trim(); 

    const hash = createHash(password);

    if (hash === target_hash) {
      console.log(`Password found: ${password}`);
      return;
    }
  }

  console.log("Password not found");
}


const target_hash = "8ec47631827329eb114714a20d9b3eeb8a9a4293bd5d6823af0632e8d4f30dfa";
//console.log(unhash(unhash(target_hash)));
breakHash(target_hash);
