#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void usage()
{
	cout << "ReaverOS initrd creator, version 0.1" << endl << endl;
	cout << "Usage:" << endl;
	cout << "./initrd <output> <list file> [<list file 2> ...]" << endl;
	cout << "<list file> means text file containing list of files to be put into initrd." << endl;
	cout << "Format of file entry in list files:" << endl;
	cout << "<original file path>" << endl;
	cout << "<destination file path> (in initrd)" << endl;
	cout << "(Yes, every entry must consist of two lines.)" << endl;
	return;
}

void zero(char * buffer, int size)
{
	for (int i = 0; i < size; i++)
		buffer[i] = 0;
	
	return;
}

// not exactly filesize in common sense, but in sectors (512 byte sectors)
int filesize(fstream * pFile)
{
	char * buffer = new char[512];
	int counter = 0;
	pFile->clear();
	pFile->seekg(0, ios::beg);
	
	while (!pFile->eof())
	{
		pFile->read(buffer, 512);
		counter++;
	}
	
	pFile->seekg(0, ios::beg);
	pFile->clear();
	return counter;
}

int main(int argc, char * * argv) 
{
	if (argc < 3)
	{
		usage();
		return 0;
	}
	
	vector<pair<fstream *, string *> > vpFiles;
	char * buffer = new char[512];
	zero(buffer, 512);
	
	for (int i = 2; i < argc; i++)
	{
		fstream input(argv[i], fstream::in);
		
		while (!input.eof())
		{
			string orig;
			input >> orig;
			
			string dest;
			input >> dest;
			
			if (input.fail())
			{
				cout << "Syntax error in file " << argv[i] << endl;
				return 0;
			}
			
			if (dest.size() > 508)
			{
				cout << "Destination filename " << dest << " is too long." << endl;
				return 0;
			}
			
			fstream * next = new fstream(orig.c_str(), fstream::in | fstream::binary);
			if (next->bad())
			{
				cout << "Could not open file " << orig << endl;
				return 0;
			}
			vpFiles.push_back(make_pair(next, new string(dest)));
		}
	}
	
	// ok, one not-nice-looking loop finished, now next, looking almost-same-awful
	// but this one uses this ugly (*it).[...] syntax x.x
	
	fstream output(argv[1], fstream::out | fstream::binary);
	
	for (vector<pair<fstream *, string *> >::iterator it = vpFiles.begin(); it != vpFiles.end(); it++)
	{
		fstream * file = (*it).first;
		string * dest = (*it).second;
		
		zero(buffer, 512);
		
		for (int i = 0; i < dest->size(); i++)
			buffer[i] = dest->c_str()[i];
		
		int size = filesize(file);
		int * destsize = (int *)(buffer + 508);
		
		(* destsize) = size;
		
		output.write(buffer, 512);
		
		if (file->eof())
			file->clear();
		file->seekg(0, ios::beg);
		
		while (!file->eof())
		{
			zero(buffer, 512);
			file->read(buffer, 512);
			output.write(buffer, 512);
		}
		
		file->close();
	}
	
	cout << "Initrd built successfully." << endl;

	return 0;
}
