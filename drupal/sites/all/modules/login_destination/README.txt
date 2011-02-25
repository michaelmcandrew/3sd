Description
-----------------
This module controls where users go after logging in.
You can configure login destination on a settings page.

It will be either static URL or a PHP snippet evaluating your own site's logic.
You can also use alias URLs instead of internal paths.
If you want to redirect a user to an URL with a query (parameters specified in GET),
you should return an array instead of a string of the following format:
    return array('path' => 'node/add/video', 'query' => 'param1=100&param2=200');

You can also configure from which pages redirection is applied.
To achieve this you can specify either a list of URLs or a PHP snippet to build this list dynamically.
So, you can redirect user to various pages depending on which pages they are logging from.


Configuration
-----------------
All configuration is performed on an Administer -> User management -> Login destination page
(admin/user/login_destination).


Examples
-----------------

PHP snippet for redirection URL should return a string. Here is an example:

  global $user;
  if ($user->uid == 1) {
    // Redirect the Administrator
    return 'admin';
  } elseif ($user->uid == 2) {
    // Redirect the Site Owner to 'create video' page
    return array('path' => 'node/add/video', 'query' => 'param1=100&param2=200');
  } else {
    return 'node';
  }

PHP snippet for Redirection condition should return boolean value. An example is:

  return ($_GET['q'] == 'user/login');

Don't put PHP tags when creating snippets.

Turn on "Preserve destination" feature to make module sensible to destination parameter.
When login from the path with specified destination (ex: 'any-page?destination=node/18')
you will be taken to node/18 instead of the URL specified in login_destination settings.

$_SESSION['login_page'] was added and stores the page you were before clicking login form button.
You can use it to determine the page you logged from instead of $_GET['q'] because $_GET['q']
always equals to 'login_redirect'. 


Authors
-----------------
Moshe Weitzman <weitzman AT tejasa DOT com>
ARDAS group <info AT ardas DOT dp DOT ua>