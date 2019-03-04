char line[] = "D53B88991F87DB6F719A5D06B9EBED76590E4ACF412BD0"; // Example DPU code.

void setup() {
  Serial.begin(115200);
  Serial.println(F("Hello world."));
  
  decodePDU( line, 46 ); // The decode function needs the character array, and how long that array is.
}

void loop() {

}


// This function decodes the PDU data to ASCII character data.
char decodePDU( char chunk[46], byte arrayLength )
{ 
  byte pos;  // The position in the character array that we're decoding.
  byte shift; // How many of the bits must be shifted over to the next byte. Since are are handling 8 bits but only need 7, we keep having one bit left over. Once there is a need to shift 7 bits, that means we have the extra 'bonus' character.
  byte nextLeftOver; // The leftover bits from the current translation
  byte prevLeftOver; // The leftover bits from the previous translation.
  byte high_nibble, low_nibble; // Used to hold the decimal values of the two hexadecimal characters from the input string.
  byte value; // The high and low nibble combined form the original byte that needs to be translated.
  byte endChar; // Hold the final translated character that is generated from the two original input hexadecimal characters.

  while( pos < arrayLength ){

    prevLeftOver = nextLeftOver;
    nextLeftOver = 0;
    
    //Serial.println(F("___________"));
    //Serial.print(F("Shift: ")); Serial.println(shift);
    //Serial.print(F("Pos: ")); Serial.println(pos);
    
    // First we turn the two hex characters into a single binary value.
    high_nibble = hex_to_int(chunk[pos]);
    //Serial.print(high_nibble); Serial.print(F(" & "));
    pos++;
    low_nibble = hex_to_int(chunk[pos]);
    //Serial.println(low_nibble);
    pos++;
    value = (high_nibble << 4) | low_nibble;
    
    //for( byte t = 8; t > 0; t-- ){ // This allows you to see the actual bits. Useful for debugging.
      //Serial.print( bitRead(value,t-1) );
    //}
    //Serial.println();
    
    // Next, we safeguard the bits we will need in the next round by copying them into a new variable.
    for( byte j = 0; j <= shift; j++ ){
      bitWrite( nextLeftOver, j, bitRead(value,(7-shift) + j) ); 
    }
    
    endChar = value; // Endchar will hold the final ASCII value.
    endChar = endChar << shift; // Scoot everything over. The bits that get destroyed have already been safeguarded.    
    endChar = endChar | prevLeftOver; // overlay the previous left over bits. This will create the final character.
    bitWrite( endChar, 7, 0 ); // Force it to be a 7-bit character by turning the 8th bit to 0. 
    Serial.print( (char) endChar);

    shift++;
    if( shift == 7 ){
      Serial.print( (char) nextLeftOver); // Once every 7 loops the left-over part will itself be 7 bits long. We can just print this as the 7th character.
      nextLeftOver = 0;
      shift = 0;
    }
  }
}

// This is a helper function. It turns a hex character (0 through F) into the actual decimal number it represents (0 through 16).
byte hex_to_int(char c){
  byte first;
  byte second;
  byte value;
 
  if (c >= 97) {
    c -= 32;
  }
  first = c / 16 - 3;
  second = c % 16;
  value = first * 10 + second;
  if (value > 9) {
    value--;
  }
  return value;
}
