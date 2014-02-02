/* Copyright (c) 2014, Inferno Embedded
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

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <flame/io.h>

namespace flame{

/**
 * A String tokenizer that corrupts the input string (changing seperators to NULLs)
 * @tparam	sep		the seperator to split on
 * 					special cases:
 * 						'w'		Whitespace ( \t\r\n)
 */
template<char sep = 'w'>
class Tokenizer {
protected:
	char		*_string;

public:
	/**
	 * Constructor
	 */
	Tokenizer() :
		_string(NULL) {}

	/**
	 * Constructor
	 * @param	data	the string to tokenize
	 */
	Tokenizer(char	*data) :
		_string (data) {};

	/**
	 * Set the string to tokenize
	 * @param	input	the new string
	 */
	void setInput(char *input) {
		_string = input;
	}

	/**
	 * Get the next token
	 * @return the next token, or NULL if there are no more
	 */
	char *nextToken() {
		if (NULL == _string) {
			return NULL;
		}

		char *start = _string;

		for (;;_string++) {
			if ('\0' == *_string) {
				if (_string > start) {
					_string = NULL;
					return start;
				} else {
					_string = NULL;
					return NULL;
				}
			}

			if ('w' == sep) {
				if (' ' == *_string || '\t' == *_string || '\r' == *_string || '\n' == *_string) {
					*_string = '\0';
					_string++;
					if (_string > start + 1) {
						return start;
					} else {
						start = _string;
						continue;
					}
				}
			} else {
				if (sep == *_string) {
					*_string = '\0';
					_string++;
					if (_string > start + 1) {
						return start;
					} else {
						start = _string;
						continue;
					}
				}
			}
		}

		UNREACHABLE;
		return NULL;
	}
};

} /* namespace flame*/
#endif /* TOKENIZER_H_ */
