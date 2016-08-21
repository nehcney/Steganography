A steganography system that can encode/decode hidden messages within HTML webpages.
--------------------------------------------------------------------------------------
This project involves coding a series of classes that enables a stegnography system. Using the classes, we are able to encode/decode secret messages within HTML webpages by first compressing the hidden string message into a series of numbers (using a Lempel-Ziv-variant compression algorithm) then converting those numbers into binary represented by tabs (1s) and spaces (0s). These tabs and spaces are then appended to the ends of each line and placed back into the webpage. Conversely, it is also able to decode webpages that have pre-encoded messages in them.

I've written the following classes:
- HashTable
- BinaryConverter
- Compressor
- Steg
- WebSteg

Additional classes include http() (written by Carey Nachenberg) which aids in downloading/uploading HTML webpages. I've also written a test.cpp file that can be used to test the functionality of the steganography system. Descriptions of each class above and how they are used are commented within their respected header and cpp files.

One last note: the computeHash() functions are located in BinaryConverter.cpp.

Full project specs can be found [here](https://github.com/nehcney/Steganography/blob/d63d171854fe3aca32e13d22e14fe6942b3a436e/P4%20-%20Spec.pdf).

Copyright (c) 2016 Yen Chen
