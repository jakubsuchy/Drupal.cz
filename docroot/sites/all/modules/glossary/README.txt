; $Id: README.txt,v 1.8.4.2.2.2 2008/05/05 14:54:36 nancyw Exp $ 
Glossary helps newbies understand the jargon which always crops up when
specialists talk about a topic. Doctors discuss CBC and EKG and CCs.
Web developers keep talking about CSS, P2P, XSLT, etc. This is all
intimidating for newbies.

The most recent documentation on this module will be found at:
http://drupal.org/node/196880

The glossary module uses a filter that scans posts for glossary terms 
(including synonyms). The glossary indicator is inserted after every
found term, or the term itself is turned into an indicator depending
on the site settings. By hovering over the indicator, users may learn
the definition of that term. Clicking the indicator leads the user to
that term presented within the whole glossary or directly to the
detailed description of the term, if available.

The glossary uses Drupal's built in taxonomy feature, so you can organize
your terms in a Drupal vocabulary. This allows you to create hierarchical
structures, synonyms and relations. Glossary terms are represented with
the taxonomy terms in the glossary vocabulary. Descriptions are used to
provide a short explanation of the terms. You can attach nodes to the
terms to provide detailed explanation on the keywords.

The Glossary module will call the Taxonomy Image module, if it's enabled,
to allow you to display an image for each term in the glossary.

If you use Firefox, you may want to install the "Longer Titles" add-on
in order to see the entire definition.

Installation
------------

1. Copy this whole folder to the appropriate modules/ directory, as usual.
   Drupal should automatically detect the module. Enable the module on
   the modules administration page.

2. Glossary terms are managed as vocabularies within the taxonomy.module.
   To get started with glossary, create a new vocabulary on the
   taxonomy administration page. The vocabulary need not be associated
   with any content types, though you can attach detailed description to terms
   by adding nodes to the terms, so it might be a good idea to associate
   the vocabulary with the "story" type. Add a few terms to the vocabulary.
   The term title should be the glossary entry, the description should be
   the explanation of that term. You can make use of the hierarchy,
   synonym, and related terms features. These features impact the display
   of the glossary when viewed in an overview. Synonyms will be flaggged in
   the content the same way as the base term. Related terms will be linked
   to each other.

3. Next, you have to set up the module and the input formats you want to use.
   This is done on the Glossary settings page at Administer >> Site configuration
   >> Glossary. First select the appropriate "General" settings and save them.
   Then select the tabs corresponding to any "input formats" you will allow to 
   be used on your site (probably at least "Filtered HTML"). You may have
   different settings for each input format, but consider that carefully. You
   will be able to choose between superscript, icon, or acronym inclusion for
   each term.

4. If your language's alphabet consists of something other than A-Z, you will
   need to enter the correct alphabet (in the correct order) on the "Alphabet"
   settings page. You may blank out the digits section if you don't use terms
   beginning with numbers.
   
5. If you want a search box on your glossary page, enable the block on admin/block page.


Advanced Usage
--------------

You can create a dedicated glossary for some pages of your site. To do so, create a new
vocabulary and put the special terms in it. Then create a new input format and add the
glossary filter to it. Then configure glossary filter in that format to look at the new
vocabulary. Finally, affiliate the special pages with this input format and you will get
the desired behavior.

You may also set up a "dictionary" that looks just like the glossary page, but is not 
associated with a vocabulary.

Authors
-------
Additional improvements and fixes by Nancy Wichmann
More improvements by Moshe Weitzman aagin. And so it goes around ...
More improvements by Frodo Looijaard <drupal [at] frodo.looijaard.name>
Many improvements by Gábor Hojtsy <goba [at] php.net>
Modified extensively by Al Maw <drupal-glossary [at] almaw.com>.
Originally written by Moshe Weitzman <weitzman [at] tejasa.com>. Much help from killes.
