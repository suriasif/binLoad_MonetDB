# binLoad_MonetDB
binary loader for MonetDB

This package comprises of three units.
1: DFD; is an XML to define the input format of the CSV, output layout, and transformations.
2:  DfdGen; the Win32 app to convert dfd (XML) to a binary format where transformations are encoded as syscalls - mini instruction set.
3: BatCsvReader; Linux/Win32 (LSB) app to read and convert CSV file to bat and heap files which are directly loadable to monetDB.

Inet (IP) fields are not supported without the patch.
https://www.monetdb.org/bugzilla/show_bug.cgi?id=3670
