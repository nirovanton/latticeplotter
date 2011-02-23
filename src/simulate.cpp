/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010 Alex Brandt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdio.h>
#include <cstdlib>

#include "simulate.h"

#include "output.h"

Simulate::Simulate(int argc, char *argv[])
: debug(false), verbose(false)
{
    using namespace boost::program_options;
    std::string usage;
    usage += "Usage: ";
    usage += argv[0];
    usage += " [options] <file_list>";
    options_description description(usage);
    variables_map variables;
    try
    {
        variables = this->parseOptions(argc, argv, &description);
    }
    catch (...)
    {
        throw SimulateArgumentError("", description);
    }

    if (variables.count("help") > 0) throw SimulateArgumentError("", description);
    this->debug = variables.count("debug") > 0;
    this->verbose = variables.count("verbose") > 0;
}

boost::program_options::variables_map Simulate::parseOptions(int argc, char *argv[], boost::program_options::options_description * description)
{
    using namespace boost::program_options;
    description->add_options()
    ("help,h", "Produce help information.")
    ("debug,d", "Turn on the debug flag to have extremely verbose output.")
    ("verbose,v", "Turn on the verbose flag to have more verbose output.")
    ;
    
    description->add_options()
    ("continuous,c", "Enable continous drawing of the plots.")
    ("next,n", "I have no idea what this could mean except to iterate?")
    ("repeat,r", "Number of times to repeat?")
    ;
    
#warning Add options here.

    variables_map variables;
    store(command_line_parser(argc, argv).options(*description).run(), variables);
    notify(variables);
    return variables;
}

void Simulate::Run()
{
#warning Implement Me!
}

SimulateError::SimulateError(const std::string &message)
: BaseError(message)
{
}

SimulateArgumentError::SimulateArgumentError(const std::string &message, const boost::program_options::options_description &description)
: BaseArgumentError(message, description)
{
}
// kate: indent-mode cstyle; space-indent on; indent-width 4;
