
=== About ===

Having been frustrated with many of the inefficiencies of the Arduino core libraries, we have built an alternate
set with performance and size in mind. Alastair has given a talk at the [Canberra Linux User Group][clug] about this
library - the slides can be viewed on [SlideShare][slide].

Much of the efficiency gains comes from addressing some of the key design deficiencies of the Arduino runtime - we
do not store pin/port information in tables in flash, needlessly preallocate objects nor dictate that excessively
large buffers must be used. Instead, control of these are handed over to the developer at compile time.

We recommend using [cppcheck][cppcheck] and the [CPPCeclipse][eclipse] Eclipse Plugin for further bug checking in your code.

=== Design Philosophies ===

 * Design for the architecture
 * Favour efficiency over simplicity
 * Hide complexity using macros
 * Provide a rich API that exposes all likely usages that are expected
 * Fix all warnings!
 * Use appropriate datatypes, and use strict typing where possible to minimise the wrong variable being passed
 * Favour C++ references over pointers to reduce bad usage which can lead to crashes
 * Leverage inheritance to make generic routines available for all objects that need it
 * Expensive operations should be asynchronous where possible, so they run in the background while your code does other things
 * Enable an event driven approach
 * BSD licensed to encourage commercial use (there is a separate GPL project for ports of GPL software to MHVLib, such as V-USB)

=== Getting Started ===

We recommend developing your MHVlib projects in Eclipse. For instructions on how to do so, please see our tutorial.

We also have another tutorial showing how to [build MHVlib using make][makemhv] from the command line.

=== Bootloader ===

If an Arduino-style bootloader is required, we recommend [Optiboot][optiboot], which clocks at only a quarter the size of the Arduino bootloader (0.5kb vs 2kb), and a programming baud rate of 115,200bps.

If USB is more your thing, you may want to consider [USBaspLoader][loader], which clocks in at 2kb (the same size as the Arduino bootloader), and pretends to be a [USBasp][usbasp] programmer. This uses software USB (based on the [V-USB library][vusblib]), removing the need for a USB->serial chip, and allows you to flash your chip over USB using [AVRdude][dude].


[clug]: http://www.clug.org.au/ "Canberra Linux User Group"
[slide]: http://www.slideshare.net/AlastairDSilva/how-to-build-a-better-arduino
[cppcheck]: http://sourceforge.net/apps/mediawiki/cppcheck/index.php?title=Main_Page
[eclipse]: http://code.google.com/a/eclipselabs.org/p/cppcheclipse/wiki/Installation
[makemhv]: http://www.makehackvoid.com/tutorials/building-mhvlib-command-line
[optiboot]: http://code.google.com/p/optiboot/
[loader]: http://www.obdev.at/products/vusb/usbasploader.html
[vusblib]: http://www.obdev.at/products/vusb/index.html
[usbasp]: http://www.fischl.de/usbasp/
[dude]: http://savannah.nongnu.org/projects/avrdude/