$Id: README.txt,v 1.1 2009/08/23 16:55:41 dww Exp $

---------------------------------
Requirements
---------------------------------
In addition to the Signup and Signup Status modules, this module requires:

* Token module: http://drupal.org/project/token


---------------------------------
Configuring Certificate Templates
---------------------------------
Certificate templates come in the form of nodes that use the token module to
substitute in profile and signup data about a given user's "completion" of a
signup.  This is useful for events like training sessions where the attendees
need certificates verifying their attendance and / or completion of the 
training.

* Create a new content type for certificate templates (i.e. "Certificate 
  Template").
* Create a new signup status code that will be used as the identifying status
  for certificated users, i.e. "Completed."
* Click on the "Certificates" tab on the Signup Status administration page.
* Choose the content type that you created for templates.
* Choose the certificate granting status code and save the settings form.
* Create a new Certificate Template.  In the "Available tokens and directives" 
  rollout, there will be the standard list of tokens, plus several 
  signup-specific tokens.  These can be used to, for example, list the user's 
  profile first and last name, the title of the event, the certificate number, 
  etc.
* Navigate to a signup-enabled node, or edit an existing one.  You will now 
  see an option for which certificate to use for completions of this event.  
  Set it to the new template.
* Change a signed-up user's status to the certificate granting status.  This
  user will now see an option to print their certificate when viewing the
  node.

