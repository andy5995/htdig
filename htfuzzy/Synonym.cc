//
// Synonym.cc
//
// Synonym: A fuzzy matching algorithm to create a database of related words
//          (or misspellings) that should be searched together.
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1995-2001 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Public License version 2 or later 
// <http://www.gnu.org/copyleft/gpl.html>
//
// $Id: Synonym.cc,v 1.8.2.8 2001/02/20 00:55:25 ghutchis Exp $
//

#ifdef HAVE_CONFIG_H
#include "htconfig.h"
#endif /* HAVE_CONFIG_H */

#include <fcntl.h>

#include "Synonym.h"
#include "htfuzzy.h"
#include "List.h"
#include "StringList.h"
#include "HtConfiguration.h"

#include <stdio.h>
#include <fstream.h>
#include <stdlib.h>
#include <sys/stat.h>

//*****************************************************************************
Synonym::Synonym(const HtConfiguration& config_arg) :
  Fuzzy(config_arg)
{
    name = "synonyms";
    db = 0;
}


//*****************************************************************************
Synonym::~Synonym()
{
    if (db)
    {
        db->Close();
        delete db;
        db = 0;
    }
}


//*****************************************************************************
int
Synonym::createDB(const HtConfiguration &config)
{
    String      tmpdir = getenv("TMPDIR");
    String 	dbFile;

    if (tmpdir.length())
      dbFile = tmpdir;
    else
      dbFile = "/tmp";

    dbFile << "/synonyms.db";

    char	input[1000];
    FILE	*fl;
	
    const String sourceFile = config["synonym_dictionary"];

    fl = fopen(sourceFile, "r");
    if (fl == NULL)
    {
	cout << "htfuzzy/synonyms: unable to open " << sourceFile << endl;
	cout << "htfuzzy/synonyms: Use the 'synonym_dictionary' attribute\n";
	cout << "htfuzzy/synonyms: to specify the file that contains the synonyms\n";
	return NOTOK;
    }

    Database	*db = Database::getDatabaseInstance(DB_HASH);

    if (db->OpenReadWrite(dbFile, 0664) == NOTOK)
    {
	delete db;
	db = 0;
	return NOTOK;
    }

    String	data;
    String	word;
    int		count = 0;
    while (fgets(input, sizeof(input), fl))
    {
	StringList	sl(input, " \t\r\n");
	for (int i = 0; i < sl.Count(); i++)
	{
	    data = 0;
	    for (int j = 0; j < sl.Count(); j++)
	    {
		if (i != j)
		    data << sl[j] << ' ';
	    }
	    word = sl[i];
	    word.lowercase();
	    data.lowercase();
	    db->Put(word, String(data.get(), data.length() - 1));
	    if (debug && (count % 10) == 0)
	    {
                cout << "htfuzzy/synonyms: " << count << ' ' << word << "\n";
		cout.flush();
	    }
	    count++;
	}
    }
    fclose(fl);
    db->Close();
    delete db;

    struct stat stat_buf;
    String mv("mv");	// assume it's in the PATH if predefined setting fails
    if ((stat(MV, &stat_buf) != -1) && S_ISREG(stat_buf.st_mode))
	mv = MV;
    system(form("%s %s %s",
		mv.get(), dbFile.get(), config["synonym_db"].get()));

    if (debug)
    {
        cout << "htfuzzy/synonyms: " << count << ' ' << word << "\n";
	cout << "htfuzzy/synonyms: Done.\n";
    }
    return OK;
}


//*****************************************************************************
int
Synonym::openIndex()
{
    const String	dbFile = config["synonym_db"];
	
    if (db)
    {
        db->Close();
        delete db;
        db = 0;
    }
    db = Database::getDatabaseInstance(DB_HASH);
    if (db->OpenRead(dbFile) == NOTOK)
    {
	delete db;
	db = 0;
	return NOTOK;
    }
    return OK;
}


//*****************************************************************************
void
Synonym::getWords(char *originalWord, List &words)
{
    String	data;
    String	stripped = originalWord;
    HtStripPunctuation(stripped);

    if (db && db->Get(stripped, data) == OK)
    {
	char	*token = strtok(data.get(), " ");
	while (token)
	{
	    words.Add(new String(token));
	    token = strtok(0, " ");
	}
    }
}
