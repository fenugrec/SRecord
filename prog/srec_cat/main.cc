/*
 *	srecord - manipulate eprom load files
 *	Copyright (C) 1998 Peter Miller;
 *	All rights reserved.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * MANIFEST: operating system entry point
 */

#include <arglex.h>
#include <srec/input/file/srecord.h>
#include <srec/memory.h>
#include <srec/memory/walker/writer.h>
#include <srec/output/file/srecord.h>
#include <srec/record.h>

#include <iostream.h>
#include <stdlib.h>
#include <vector>


class arglex_here: public arglex
{
public:
	enum
	{
		token_output = arglex::token_MAX,
	};

	arglex_here(int, char **);
};

arglex_here::arglex_here(int ac, char **av)
	: arglex(ac, av)
{
	static table_ty table[] =
	{
		{ "-Output", token_output, },
		ARGLEX_END_MARKER
	};

	table_set(table);
}


static void
usage(const char *progname)
{
	cerr << "Usage: " << progname
		<< " [ <option>...][ <infile> [ <outfile> ]]" << endl;
	exit(1);
}


int
main(int argc, char **argv)
{
	arglex_here cmdline(argc, argv);
	cmdline.token_next();
	typedef vector<const char *> infile_t;
	infile_t infile;
	const char *outfile = 0;
	while (cmdline.token_cur() != arglex::token_eoln)
	{
		switch (cmdline.token_cur())
		{
		case arglex::token_option:
			cerr << "Unknown ``" << cmdline.value_string()
				<< "'' option" << endl;
			usage(argv[0]);

		default:
			cerr << "Misplaced ``" << cmdline.value_string()
				<< "'' option" << endl;
			usage(argv[0]);

		case arglex::token_string:
			infile.push_back(cmdline.value_string());
			break;

		case arglex::token_stdio:
			infile.push_back("-");
			break;

		case arglex_here::token_output:
			if (outfile)
				usage(argv[0]);
			switch (cmdline.token_next())
			{
			default:
				usage(argv[0]);

			case arglex::token_string:
				outfile = cmdline.value_string();
				break;

			case arglex::token_stdio:
				outfile = "-";
				break;
			}
			break;
		}
		cmdline.token_next();
	}
	if (infile.size() == 0)
		infile.push_back("-");
	if (!outfile)
		outfile = "-";

	/*
	 * Read the input into memory.	This allows the data to be
	 * consolidated on output, and warnings to be issued for
	 * duplicates.
	 *
	 * It is assumed the data will all fit into memory.  This is
	 * usually reasonable, because these utilities are used for
	 * eproms which are usualloy smaller than the available virtual
	 * memory of the development system.
	 */
	srec_memory *mp = new srec_memory();
	srec_record::address_t taddr = 0;
	for (infile_t::iterator it = infile.begin(); it != infile.end(); ++it)
	{
		srec_input *ifp = new srec_input_file_srecord(*it);
		srec_record record;
		while (ifp->read(record))
		{
			switch (record.get_type())
			{
			case srec_record::type_unknown:
			case srec_record::type_header:
			case srec_record::type_data_count:
				break;

			case srec_record::type_data:
				for (int j = 0; j < record.get_length(); ++j)
				{
					srec_record::address_t address =
						record.get_address() + j;
					if (mp->set_p(address))
					{
						ifp->warning
						(
							"duplicate %08lX value",
							(long)address
						);
					}
					mp->set(address, record.get_data(j));
				}
				break;

			case srec_record::type_termination:
				if (record.get_address() > taddr)
					taddr = record.get_address();
				break;
			}
		}
		delete ifp;
	}

	/*
	 * Open the output file and write the remembered data out to it.
	 */
	srec_output *ofp = new srec_output_file_srecord(outfile);
	ofp->write_header();
	srec_memory_walker *w = new srec_memory_walker_writer(ofp);
	mp->walk(w);
	ofp->write_termination(taddr);
	delete ofp;

	/*
	 * Dispose of the memory image of the data.
	 * (Probably not necessary.)
	 */
	delete mp;

	/*
	 * success
	 */
	exit(0);
	return 0;
}
