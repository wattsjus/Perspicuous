Here you would put all the public facing content.  Everything in the root folder takes the fallover next to the Clients/Default folder then if there are any files in the Clients/{subdomain} folder that takes the top precedence.
all files here will be views (.php files)

if you want to use a data object you'd make a new instance or load an instance of the object you want and create web controls:

<?
$a = new Example();
Form::Start(Test);
echo Form::Text($a->Name);
Form::End();
?>

This will create a text box that will allow you to easily bind to a javascript view object via knockout if you wish.  The form class will start and end your forms much like Microsoft's Razor syantax.
There are many different types of controls you can use and the objects that are create are really classes that can be tostringed and printed natively.