//
//      srecord - manipulate eprom load files
//      Copyright (C) 2003, 2006-2008 Peter Miller
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program. If not, see
//      <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDE_SREC_INPUT_FILE_FAIRCHILD_H
#define INCLUDE_SREC_INPUT_FILE_FAIRCHILD_H

#include <lib/srec/input/file.h>

/**
  * The srec_input_file_fairchild class is used to represent the parse
  * stat ewhen reading a file in Fairchild Fairbug format.
  */
class srec_input_file_fairchild:
    public srec_input_file
{
public:
    /**
      * The destructor.
      */
    virtual ~srec_input_file_fairchild();

private:
    /**
      * The constructor.
      *
      * @param file_name
      *     The name of the file to be read.
      */
    srec_input_file_fairchild(const std::string &file_name);

public:
    /**
      * The create class method is used to create new dynamically
      * allocated instances of this class.
      *
      * @param file_name
      *     The name of the file to be read.
      * @returns
      *     smart pointer to new instance
      */
    static pointer create(const std::string &file_name);

protected:
    // See base class for documentation.
    bool read(srec_record &record);

    // See base class for documentation.
    const char *get_file_format_name() const;

private:
    /**
      * The get_nibble method gets a single hex-digit from input.
      * We override the one in the base class because the checksum is
      * nibble-based, not byte-based.
      */
    int get_nibble();

    /**
      * The get_byte method gets a two hex-digit from input and assembles
      * them (big-endian) into a byte.  We override the one in the base
      * class because the checksum is nibble-based, not byte-based.
      */
    int get_byte();

    /**
      * The header_seen instance variable is used to member whether the
      * file header has been processed yet.
      */
    bool header_seen;

    /**
      * The address instance variable is used to member the address at
      * the currebt point in the input file.
      */
    unsigned long address;

    /**
      * The file_contains_data instance variable is used to member
      * whether any data records have been seen yet.
      */
    bool file_contains_data;

    /**
      * The default constructor.  Do not use.
      */
    srec_input_file_fairchild();

    /**
      * T  Do not use.he copy constructor.  Do not use.
      */
    srec_input_file_fairchild(const srec_input_file_fairchild &);

    /**
      * The assignment operator.
      */
    srec_input_file_fairchild &operator=(const srec_input_file_fairchild &);
};

#endif // INCLUDE_SREC_INPUT_FILE_FAIRCHILD_H
