#!/usr/bin/perl -w

# Generate MHVLib headers based on AVRLibC headers

use English;
use strict;
use List::Util qw(max);
use File::Temp ();
use File::Temp qw/ :seekable /;
use FileHandle;
use feature 'switch';

my $topDir;

my $timers = 6;
my $usarts = 4;

sub usage() {
	print "Generate the headers for a chip\n";
	print "If no chip is specified, all chips supported by the compiler will be generated\n";
	print "Usage:\n";
	print "\tgenerate_io_headers <top directory> [avr chip identifier] [avr chip identifier] ...";
	die "\n";
} ## end sub usage()

##
# Output a line to evaluate a macro
# @param    handle  the handle to write to
# @param    name    the name of the sacro to evaluate
sub evaluateMacro($$) {
	my ($handle, $name) = @ARG;

	print $handle "\"$name\" = $name;\n";
}

##
# Run the C preprocessor to generate the preprocessed output that we care about
# @param    chip    the chip to run for
# @return a filehandle to the preprocessed output
sub runPreprocessor($) {
	my ($chip) = @ARG;

	my $srcFile = new File::Temp(
		TEMPLATE => 'genioXXXXX',
		UNLINK   => 0,
		SUFFIX   => '.c'
	) or die $OS_ERROR;

	print $srcFile "#include <avr/io.h>\n";

	for (my $timer = 0; $timer < $timers; $timer++) {
		evaluateMacro($srcFile, "TCCR${timer}A");
		evaluateMacro($srcFile, "TCCR${timer}B");
		evaluateMacro($srcFile, "TCCR${timer}C");
		evaluateMacro($srcFile, "OCR${timer}A");
		evaluateMacro($srcFile, "OCR${timer}B");
		evaluateMacro($srcFile, "OCR${timer}C");
		evaluateMacro($srcFile, "TCNT${timer}");
		evaluateMacro($srcFile, "TIMSK${timer}");
		evaluateMacro($srcFile, "ICR${timer}");
		evaluateMacro($srcFile, "OCIE${timer}A");
		evaluateMacro($srcFile, "TIMER${timer}_COMPA_vect");
		evaluateMacro($srcFile, "TIMER${timer}_COMPC_vect");
		evaluateMacro($srcFile, "SIG_OUTPUT_COMPARE{timer}A");
	} ## end for (my $timer = 0; $timer...

	evaluateMacro($srcFile, "TIMSK");

	for (my $usart = 0; $usart < $usarts; $usart++) {
		evaluateMacro($srcFile, "USART${usart}_RX_vect");
		evaluateMacro($srcFile, "UBRR${usart}");
		evaluateMacro($srcFile, "UCSR${usart}A");
		evaluateMacro($srcFile, "UCSR${usart}B");
		evaluateMacro($srcFile, "UCSR${usart}C");
		evaluateMacro($srcFile, "UDR${usart}");
	} ## end for (my $usart = 0; $usart...
	evaluateMacro($srcFile, "USART_RX_vect");
	evaluateMacro($srcFile, "UBRR");
	evaluateMacro($srcFile, "UBRRL");
	evaluateMacro($srcFile, "UDR");
	evaluateMacro($srcFile, "UCSRA");
	evaluateMacro($srcFile, "UCSRB");
	evaluateMacro($srcFile, "UCSRC");

	foreach my $port (qw 'A B C D E F G H I J K L M N O P Q R S T U V W X Y Z') {
		evaluateMacro($srcFile, "PIN$port");
		evaluateMacro($srcFile, "DDR$port");
		evaluateMacro($srcFile, "PORT$port");

		for (my $pin = 0; $pin < 8; $pin++) {
			evaluateMacro($srcFile, "PIN$port$pin");
		}
	} ## end foreach my $port (qw 'A B C D E F G H I J K L M N O P Q R S T U V W X Y Z')

	evaluateMacro($srcFile, "EE_READY_VECT");
	evaluateMacro($srcFile, "EEPROM_READY_VECT");

	$srcFile->close();

	my $command = "avr-gcc -E -mmcu=$chip $srcFile";
	open(my $output, '-|', $command)
	or die "Could not execute '$command': $OS_ERROR";

	sleep 1;
	unlink $srcFile;

	return $output;
} ## end sub runPreprocessor($)

##
# Clean a value
# @param    value   the value to clean
# @return the cleaned value
sub cleanValue($) {
	my ($value) = @ARG;

	$value =~ s/\*\(volatile uint(8|16)_t \*\)//;
	if ($value =~ /0x/i) {
		$value = sprintf "0x%x", eval $value;
	}

	return $value;
} ## end sub cleanValue($)

##
# Generate a hash mapping macros to their values
# @param    chip    the chip to map macros for
# @return a hash mapping macros to their values
sub evaluateMacros($) {
	my ($chip) = @ARG;

	my %macros;

	my $preprocessed = runPreprocessor($chip);
	while (<$preprocessed>) {
		chomp;
		if (/\"(.*)\" = (.*);/) {
			my $macro = $1;
			my $value = $2;

			if ($macro eq $value) {
				next;
			}
			$macros{$macro} = cleanValue($value);

#           print "'$macro' = '$macros{$macro}'\n";
		} ## end if (/\"(.*)\" = (.*);/)
	} ## end while (<$preprocessed>)

	return %macros;
} ## end sub evaluateMacros($)

##
# Emit headers
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit timer declations for
sub emitHeaders($$) {
	my ($handle, $chip) = @ARG;

	$chip = uc $chip;

	print $handle <<"EOF"
/*
 * Copyright (c) 2012, Make, Hack, Void Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Make, Hack, Void nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MAKE, HACK, VOID BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef MHV_IO_${chip}_H_
#define MHV_IO_${chip}_H_

#include <avr/io.h>

#ifdef INT0_vect
#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

EOF
} ## end sub emitHeaders($$)

##
# Emit Footers
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit timer declations for
sub emitFooters($$) {
	my ($handle, $chip) = @ARG;

	$chip = uc $chip;

	print $handle <<"EOF"

#endif // MHV_IO_${chip}_H_

EOF
} ## end sub emitFooters($$)

##
# Determine how many bits and prescalers a timer has
# @param    chip    the chip hosting the timer
# @param    timer   the timer number
# @return (bits, prescalers)
sub timerInfo($$) {
	my ($chip, $timer) = @ARG;

	if (0 == $timer) {
		return (8, 5);
	}

	if (2 == $timer) {
		return (8, 7);
	}

	return (16, 5);
} ## end sub timerInfo($$)

##
# Emit timer declarations
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit timer declations for
# @param    macros  the evaluated macros for the chip
sub emitTimers($$%) {
	my ($handle, $chip, %macros) = @ARG;

	print $handle <<"EOF";
//\t\t\t\t\t\tbits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
EOF

	for (my $timer = 0; $timer < $timers; $timer++) {
		if (defined $macros{"TCCR${timer}A"}) {
			my ($bits, $prescaler) = timerInfo($chip, $timer);
			my $paddedBits = pack 'A5', "$bits,";
			$prescaler = pack 'A33', "TimerType::HAS_${prescaler}_PRESCALERS,";

			my $ctrlRegA = pack 'A9', $macros{"TCCR${timer}A"} . ',';
			my $ctrlRegB = pack 'A9', $macros{"TCCR${timer}B"} . ',';
			my $ctrlRegC = $macros{"TCCR${timer}C"};
			$ctrlRegC //= 0;
			$ctrlRegC = pack 'A9', $ctrlRegC . ',';

			my $overflow1 = pack 'A10', $macros{"OCR${timer}A"} . ',';
			my $overflow2 = pack 'A10', $macros{"OCR${timer}B"} . ',';
			my $overflow3 = $macros{"OCR${timer}C"};
			$overflow3 //= 0;
			$overflow3 = pack 'A10', $overflow3 . ',';

			my $inputCapture1 = $macros{"ICR${timer}"};
			$inputCapture1 //= 0;
			$inputCapture1 = pack 'A14', $inputCapture1 . ',';
			my $counter = pack 'A8', $macros{"TCNT${timer}"} . ',';
			my $timsk = $macros{"TIMSK${timer}"};
			$timsk //= $macros{'TIMSK'};
			my $interrupt = pack 'A10', $timsk . ',';
			my $intEnable = $macros{"OCIE${timer}A"};

			print $handle <<"EOF";
#define MHV_TIMER${bits}_${timer}\t$paddedBits$prescaler$ctrlRegA$ctrlRegB$ctrlRegC$overflow1$overflow2$overflow3$inputCapture1$counter$interrupt$intEnable
EOF
		} ## end if (defined $macros{"TCCR${timer}A"...
	} ## end for (my $timer = 0; $timer...

	print $handle "\n\n";

	for (my $timer = 0; $timer < $timers; $timer++) {
		if (defined $macros{"TIMER${timer}_COMPC_vect"}) {
			print $handle <<"EOF";
#define MHV_TIMER${timer}_INTERRUPTS TIMER${timer}_COMPA_vect, TIMER${timer}_COMPB_vect, TIMER${timer}_COMPC_vect
EOF
		}
		elsif (defined $macros{"TIMER${timer}_COMPA_vect"}) {
			print $handle <<"EOF";
#define MHV_TIMER${timer}_INTERRUPTS TIMER${timer}_COMPA_vect, TIMER${timer}_COMPB_vect, 0
EOF
		}
		elsif (defined $macros{"SIG_OUTPUT_COMPARE${timer}A"}) {
			print $handle <<"EOF";
#define MHV_TIMER${timer}_INTERRUPTS SIG_OUTPUT_COMPARE${timer}A, SIG_OUTPUT_COMPARE${timer}B, 0
EOF
		}
	} ## end for (my $timer = 0; $timer...

	print $handle "\n\n";
} ## end sub emitTimers($$%)

##
# Emit USART declarations
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit timer declations for
# @param    macros  the evaluated macros for the chip
sub emitUSARTs($$%) {
	my ($handle, $chip, %macros) = @ARG;

	print $handle <<"EOF";
// USART\t\t\tBaud   Status Control I/O
//      \t\t\tubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
EOF

	for (my $usart = 0; $usart < $usarts; $usart++) {
		if (defined $macros{"UDR${usart}"}) {
			my $ubrr = pack 'A7', $macros{"UBRR${usart}"} . ',';
			my $ucsra = $macros{"UCSR${usart}A"};
			$ucsra //= $macros{"UCSRA"};
			my $status = pack 'A7', $ucsra . ',';
			my $ucsrb = $macros{"UCSR${usart}B"};
			$ucsrb //= $macros{"UCSRB"};
			my $controlB = pack 'A8', $ucsrb . ',';
			my $ucsrc = $macros{"UCSR${usart}C"};
			$ucsrc //= $macros{"UCSRC"};
			my $controlC = pack 'A8', $ucsrc . ',';
			my $io      = pack 'A7', $macros{"UDR${usart}"} . ',';

			print $handle <<"EOF";
#define MHV_USART${usart}\t$ubrr$status$controlB$controlC${io}RXEN${usart}, TXEN${usart}, RXCIE${usart}, TXCIE${usart}, UDRE${usart}, U2X${usart}
EOF
		} ## end if (defined $macros{"UDR${usart}"...
	} ## end for (my $usart = 0; $usart...

	if (defined $macros{"UDR"}) {
		my $ubrr = $macros{"UBRR"};
		$ubrr //= $macros{"UBRRL"};
		$ubrr = pack 'A7', $ubrr . ',';
		my $status  = pack 'A7', $macros{"UCSRA"} . ',';
		my $controlB = pack 'A8', $macros{"UCSRB"} . ',';
		my $controlC = pack 'A8', $macros{"UCSRC"} . ',';
		my $io      = pack 'A7', $macros{"UDR"} . ',';

		print $handle <<"EOF";
#define MHV_USART0\t$ubrr$status$controlB$controlC${io}RXEN,  TXEN,  RXCIE,  TXCIE,  UDRE,  U2X
EOF
	} ## end if (defined $macros{"UDR"...

	print $handle "\n\n";

	for (my $usart = 0; $usart < $usarts; $usart++) {
		if (defined $macros{"USART${usart}_RX_vect"}) {
			print $handle <<"EOF";
#define MHV_USART${usart}_INTERRUPTS\tUSART${usart}_RX_vect, USART${usart}_TX_vect
EOF
		}
	} ## end for (my $usart = 0; $usart...

	if (defined $macros{"USART_RX_vect"}) {
		print $handle <<"EOF";
#define MHV_USART0_INTERRUPTS\tUSART_RX_vect, USART_TX_vect
EOF
	}

	print $handle "\n\n";
} ## end sub emitUSARTs($$%)

##
# Emit ADC voltage References
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit the header for
sub emitADCReference($$) {
	my ($handle, $chip) = @ARG;

	my %refs;

	given ($chip) {
		when (/attiny(2|4)313/) { }
		when (/attiny(2|4|8)5/) {
			%refs = (
				'0X00 << 4' => 'VCC',
				'0x04 << 4' => 'AREF',
				'0x08 << 6' => 'REF1V1',
				'0x09 << 6' => 'REF2V56',
				'0x0d << 6' => 'REF2V56_AREF',
			);

		} ## end when (/attiny(2|4|8)5/)
		when (/atmega((4|8|16)8P?A?|328P?)/) {
			%refs = (
				'0x00 << 6' => 'AREF',
				'0x01 << 6' => 'AVCC',
				'0x03 << 6' => 'REF1V1',
			);
		} ## end when (/atmega((4|8|16)8P?A?|328P?)/)
		when (/atmega(640|1280|1281|2560|2561)/) {
			%refs = (
				'0x00 << 6' => 'AREF',
				'0x01 << 6' => 'AVCC',
				'0x02 << 6' => 'REF1V1',
				'0x03 << 6' => 'REF2V56',
			);
		} ## end when (/atmega(640|1280|1281|2560|2561)/)

		default {
			die "Unsupported chip $chip";
		}
	} ## end given

	my $count = keys %refs;
	if (0 == $count) {
		return;
	}

	print $handle <<"EOF";
enum class ADCReference {
EOF

	foreach my $value (sort keys %refs) {
		my $reference = $refs{$value};

		print $handle "\t${reference}\t= ($value)";

		if (0 != --$count) {
			print $handle ',';
		}

		print $handle "\n";
	} ## end foreach my $value (sort keys...

	print $handle <<"EOF";
};

EOF

} ## end sub emitADCReference($$)

##
# Emit ADC channels
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit the header for
sub emitADCChannels($$) {
	my ($handle, $chip) = @ARG;

	my %channels;

	given ($chip) {
		when (/attiny(2|4)313/) { }
		when (/attiny(2|4|8)5/) {
			%channels = (
				'0x00' => '0',
				'0x01' => '1',
				'0x02' => '2',
				'0x03' => '3',
				'0x04' => '2_X1_2',
				'0x05' => '2_X20_2',
				'0x06' => '2_X1_3',
				'0x07' => '2_X20_3',
				'0x08' => '0_X1_0',
				'0x09' => '0_X20_0',
				'0x0a' => '0_X1_1',
				'0x0b' => '0_X20_1',
				'0x0c' => 'V_BANDGAP',
				'0x0d' => '0V',
				'0x0f' => 'TEMPERATURE',
			);

		} ## end when (/attiny(2|4|8)5/)
		when (/atmega((4|8|16)8P?A?|328P?)/) {
			%channels = (
				'0x00' => '0',
				'0x01' => '1',
				'0x02' => '2',
				'0x03' => '3',
				'0x04' => '4',
				'0x05' => '5',
				'0x06' => '6',
				'0x07' => '7',
				'0x08' => '8',
				'0x08' => 'TEMPERATURE',
				'0xfe' => '1V1',
				'0xff' => '0V',
			);
		} ## end when (/atmega((4|8|16)8P?A?|328P?)/)
		when (/atmega(640|1280|1281|2560|2561)/) {
			%channels = (
				'0x00' => '0',
				'0x01' => '1',
				'0x02' => '2',
				'0x03' => '3',
				'0x04' => '4',
				'0x05' => '5',
				'0x06' => '6',
				'0x07' => '7',
				'0x08' => '0_X10_0',
				'0x09' => '1_X10_0',
				'0x0a' => '0_X200_0',
				'0x0b' => '1_X200_0',
				'0x0c' => '2_X10_2',
				'0x0d' => '3_X10_2',
				'0x0e' => '2_X200_2',
				'0x0f' => '3_X200_2',
				'0x10' => '0_X1_1',
				'0x11' => '1_X1_1',
				'0x12' => '2_X1_1',
				'0x13' => '3_X1_1',
				'0x14' => '4_X1_1',
				'0x15' => '5_X1_1',
				'0x16' => '6_X1_1',
				'0x17' => '7_X1_1',
				'0x18' => '0_X1_2',
				'0x19' => '1_X1_2',
				'0x1a' => '2_X1_2',
				'0x1b' => '3_X1_2',
				'0x1c' => '4_X1_2',
				'0x1d' => '5_X1_2',
				'0x1e' => '1V1',
				'0x1f' => '0V',
				'0x20' => '8',
				'0x21' => '9',
				'0x22' => '10',
				'0x23' => '11',
				'0x24' => '12',
				'0x25' => '13',
				'0x26' => '14',
				'0x27' => '15',
				'0x28' => '8_X10_8',
				'0x29' => '9_X10_8',
				'0x2a' => '8_X200_8',
				'0x2b' => '9_X200_8',
				'0x2c' => '10_X10_10',
				'0x2d' => '11_X10_10',
				'0x2e' => '10_X200_10',
				'0x2f' => '11_X200_10',
				'0x30' => '8_X1_9',
				'0x31' => '9_X1_9',
				'0x32' => '10_X1_9',
				'0x33' => '11_X1_9',
				'0x34' => '12_X1_9',
				'0x35' => '13_X1_9',
				'0x36' => '14_X1_9',
				'0x37' => '15_X1_9',
				'0x38' => '8_X1_10',
				'0x39' => '9_X1_10',
				'0x3a' => '10_X1_10',
				'0x3b' => '11_X1_10',
				'0x3c' => '12_X1_10',
				'0x3d' => '13_X1_10',
			);
		} ## end when (/atmega(640|1280|1281|2560|2561)/)

		default {
			die "Unsupported chip $chip";
		}
	} ## end given

	my $count = keys %channels;

	if (0 == $count) {
		return;
	}

	print $handle <<"EOF";
enum class ADCChannel {
EOF

	print $handle "\tUNDEFINED   = 0xff,\n";

	foreach my $value (sort keys %channels) {
		my $channel = $channels{$value};

		print $handle "\tCHANNEL_${channel}   = $value";

		if (0 != --$count) {
			print $handle ',';
		}

		print $handle "\n";
	} ## end foreach my $value (sort keys...

	print $handle <<"EOF";
};

EOF

} ## end sub emitADCChannels($$)

##
# Get the pinchange interrupt hash for a chip
# @param    chip    the chip to emit the header for
# @return the pinchange interrupt hash
sub getPinchangeInterruptTable($) {
	my ($chip) = @ARG;

	my %interrupts;

	given ($chip) {
		when (/attiny(2|4)313/) {
			%interrupts = (
				'B0' => 0,
				'B1' => 1,
				'B2' => 2,
				'B3' => 3,
				'B4' => 4,
				'B5' => 5,
				'B6' => 6,
				'B7' => 7,
				'A0' => 8,
				'A1' => 9,
				'A2' => 10,
				'D0' => 11,
				'D1' => 12,
				'D2' => 13,
				'D3' => 14,
				'D4' => 15,
				'D5' => 16,
				'D6' => 17,
			);
		} ## end when (/attiny(2|4)313/)
		when (/attiny(2|4|8)5/) {
			%interrupts = (
				'B0' => 0,
				'B1' => 1,
				'B2' => 2,
				'B3' => 3,
				'B4' => 4,
				'B5' => 5,
			);
		} ## end when (/attiny(2|4|8)5/)
		when (/atmega((4|8|16)8P?A?|328P?)/) {
			%interrupts = (
				'B0' => 0,
				'B1' => 1,
				'B2' => 2,
				'B3' => 3,
				'B4' => 4,
				'B5' => 5,
				'B6' => 6,
				'B7' => 7,
				'C0' => 8,
				'C1' => 9,
				'C2' => 10,
				'C3' => 11,
				'C4' => 12,
				'C5' => 13,
				'C6' => 14,
				'D0' => 16,
				'D1' => 17,
				'D2' => 18,
				'D3' => 19,
				'D4' => 20,
				'D5' => 21,
				'D6' => 22,
				'D7' => 23,
			);
		} ## end when (/atmega((4|8|16)8P?A?|328P?)/)
		when (/atmega(640|1280|1281|2560|2561)/) {
			%interrupts = (
				'B0' => 0,
				'B1' => 1,
				'B2' => 2,
				'B3' => 3,
				'B4' => 4,
				'B5' => 5,
				'B6' => 6,
				'B7' => 7,
				'E8' => 8,
				'J0' => 9,
				'J1' => 10,
				'J2' => 11,
				'J3' => 12,
				'J4' => 13,
				'J5' => 14,
				'J6' => 15,
				'K0' => 16,
				'K1' => 17,
				'K2' => 18,
				'K3' => 19,
				'K4' => 20,
				'K5' => 21,
				'K6' => 22,
				'K7' => 23,
			);
		} ## end when (/atmega(640|1280|1281|2560|2561)/)

		default {
			die "Unsupported chip $chip";
		}
	} ## end given

	return %interrupts;
} ## end sub getPinchangeInterruptTable($)

##
# Emit pin declarations
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit the header for
# @param    macros  the evaluated macros for the chip
sub emitPins($$%) {
	my ($handle, $chip, %macros) = @ARG;
	my %interruptTable = getPinchangeInterruptTable($chip);

	print $handle "//                   Dir,   Output, Input,  Bit,PCINT\n";

	foreach my $port (qw 'A B C D E F G H I J K L M N O P Q R S T U V W X Y Z') {
		for (my $pin = 0; $pin < 8; $pin++) {
			if (defined $macros{"PIN$port$pin"}) {
				my $ddr = pack 'A8', $macros{"DDR$port"} . ',';
				my $out = pack 'A8', $macros{"PORT$port"} . ',';
				my $in  = pack 'A8', $macros{"PIN$port"} . ',';
				my $bit = pack 'A4', $pin . ',';
				my $pcInt = $interruptTable{"$port$pin"};
				$pcInt //= -1;

				print $handle "#define MHV_PIN_$port$pin\t$ddr$out$in$bit$pcInt\n";
			} ## end if (defined $macros{"PIN$port$pin"...
		} ## end for (my $pin = 0; $pin ...
	} ## end foreach my $port (qw 'A B C D E F G H I J K L M N O P Q R S T U V W X Y Z')

	print $handle "\n#define MHV_PC_INT_COUNT\t" . (max(values %interruptTable) + 1) . "\n\n";
} ## end sub emitPins($$%)

##
# Emit timer pins
# @param    handle  the filehandle to write to
# @param    chip    the chip to emit the header for
sub emitTimerPins($$%) {
	my ($handle, $chip) = @ARG;

	my %timerPins;

	given ($chip) {
		when (/attiny(2|4)313/) {
			%timerPins = (
				'0A' => 'B2',
				'0B' => 'D5',
				'1A' => 'B3',
				'1B' => 'B4',
			);
		} ## end when (/attiny(2|4)313/)
		when (/attiny(2|4|8)5/) {
			%timerPins = (
				'0A' => 'B0',
				'0B' => 'B1',
				'1A' => 'B1',
				'1B' => 'B4',
			);
		} ## end when (/attiny(2|4|8)5/)
		when (/atmega((4|8|16)8P?A?|328P?)/) {
			%timerPins = (
				'0A' => 'D6',
				'0B' => 'D5',
				'1A' => 'B1',
				'1B' => 'B2',
				'2A' => 'B3',
				'2B' => 'D3',
			);
		} ## end when (/atmega((4|8|16)8P?A?|328P?)/)
		when (/atmega(640|1280|1281|2560|2561)/) {
			%timerPins = (
				'0A' => 'B7',
				'0B' => 'G5',
				'1A' => 'B5',
				'1B' => 'B6',
				'1C' => 'B7',
				'2A' => 'B4',
				'2B' => 'H6',
				'3A' => 'E3',
				'3B' => 'E4',
				'3C' => 'E5',
				'4A' => 'H3',
				'4B' => 'H4',
				'4C' => 'H5',
				'5A' => 'L3',
				'5B' => 'L4',
				'5C' => 'L6',
			);
		} ## end when (/atmega(640|1280|1281|2560|2561)/)

		default {
			die "Unsupported chip $chip";
		}
	} ## end given

	for (my $timer = 0; $timer < $timers; $timer++) {
		foreach my $channel ('A', 'B', 'C') {
			my $pin = $timerPins{"$timer$channel"};
			if (defined $pin) {
				print $handle "\n#define MHV_PIN_TIMER_${timer}_${channel}\tMHV_PIN_$pin";
			}
		} ## end foreach my $channel ('A', 'B'...
	} ## end for (my $timer = 0; $timer...

	print $handle "\n\n";

} ## end sub emitTimerPins($$%)

##
# Generate the header for a chip
# @param    chip    the chip to emit the header for
sub emitChip($) {
	my ($chip) = @ARG;

	my $header = new FileHandle("$topDir/chips/MHV_io_$chip.h", 'w')
	or die "Could not open header: $OS_ERROR";

	print "Generating header for chip '$chip'\n";

	emitHeaders($header, $chip);

	my %macros = evaluateMacros($chip);

	emitTimers($header, $chip, %macros);

	emitUSARTs($header, $chip, %macros);

	emitADCReference($header, $chip);

	emitADCChannels($header, $chip);

	emitPins($header, $chip, %macros);

	emitTimerPins($header, $chip, %macros);

	emitFooters($header, $chip);
} ## end sub emitChip($)

##
# Is a chip supported by MHVLib?
# (does it have the appropriate tables in this file?)
# @param    chip    the chip to check
sub isChipSupported($) {
	my ($chip) = @ARG;

	given ($chip) {
		when (/attiny(2|4)313/) {
			return 1;
		}
		when (/attiny(2|4|8)5/) {
			return 1;
		}
		when (/atmega((4|8|16)8P?A?|328P?)/) {
			return 1;
		}
		when (/atmega(640|1280|1281|2560|2561)/) {
			return 1;
		}

		default {
			return 0;
		}
	} ## end given
} ## end sub isChipSupported($)

##
# Get the list of chips supported by the compiler
# @return a list of the chips supported by the compiler
sub getChipList() {
	my $command = "avr-gcc -mmcu=? 2>&1";
	open(my $output, '-|', $command)
	or die "Could not execute '$command': $OS_ERROR";

	my @chips;

	while (<$output>) {
		my @bits = split / /;
		foreach (@bits) {
			if (/^at/) {
				push @chips, $ARG;
			}
		}
	} ## end while (<$output>)

	return @chips;
} ## end sub getChipList()

##
# Emit a header to switch between chips
# @param    chips   a list of chips to switch between
sub emitChipList(@) {
	my @chips = @ARG;

	my $header = new FileHandle("$topDir/chips/MHV_io_chip.h", 'w')
	or die "Could not open header: $OS_ERROR";

	emitHeaders($header, 'CHIP');

	do {
		my $chip      = shift @chips;
		my $macroName = uc "__AVR_${chip}__";
		$macroName =~ s/MEGA/mega/;
		$macroName =~ s/TINY/tiny/;
		print $header <<"EOF";
#if defined($macroName)
#include <chips/MHV_io_$chip.h>
EOF
	};

	foreach my $chip (@chips) {
		my $macroName = uc "__AVR_${chip}__";
		$macroName =~ s/MEGA/mega/;
		$macroName =~ s/TINY/tiny/;
		print $header <<"EOF";
#elif defined($macroName)
#include <chips/MHV_io_$chip.h>
EOF
	} ## end foreach my $chip (@chips)

	print $header <<"EOF";
#else
#error unknown chip
#endif
EOF

	emitFooters($header, 'CHIP');
} ## end sub emitChipList(@)

$topDir = shift @ARGV;
if (!-d $topDir) {
	usage;
}

my @chips = @ARGV;

if (!@chips) {
	@chips = getChipList();
}

my @supportedChips;
foreach my $chip (@chips) {
	if (isChipSupported($chip)) {
		push @supportedChips, $chip;
	}
}

emitChipList(@supportedChips);

foreach my $chip (@supportedChips) {
	emitChip($chip);
}

