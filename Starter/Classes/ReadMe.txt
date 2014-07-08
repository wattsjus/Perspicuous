Here you would add your classes that can be accessed through any page in the public or private folders.

You have two built-in choices here.  You can inherit the base DataObject class and your config file will know what db type to use in this case there is only MySqlDB but is built to handle any kind of db you may need.  Just need to copy the base file (Core/Plugins/MySqlDB.inc) and change all the functions to accomidate your db type.

The other option is to create a table standard.  I've found one that I like that actually uses a binary guid as the primary key named GuidUpdateAndCreate (Core\Plugins\TableStandards.inc) here you can see some of the standard markup you can make in your classes when writing data classes.  If you like this standard you can create a class as follows:

class Example extends GuidUpdateAndCreate { 
/**
Required
Guid
*/
public $ExampleId = '';
/**
Required
Length(10,20)
*/
public $Name = '';
}

In this example this utility infers that ExampleId is the primary key and will generate a new GUID if the property ExampleId == '' and insert or update accordingly.


$e = new Example();
$e->Name = 'Test';
$e->Save();

In this example the utility will save a new record in the Example table with the name Test with a unique binary GUID in the ExampleID filed and all those standard fields in the GuidUpdateAndCreate class will be populated accordingly.

Querying:

Now I did write a way to use a Microsoft like syantax (lync)

Lets say we have a few tables that can join to the Example table like Foo and Bar.

$results = Example::Join(Foo::On(ExampleId))->Join(Bar::On(ExampleId))->Where(Foo::Name()->Equals(Name))->Select();

In this result we would get all records that have Foo.Name = 'Name' with the tables Example, Foo, and Bar.

You can retrieve the data in the tables like this:

foreach($results as $key => $val) {
$TempName = $val->Name->Value
}
For every unique field name you can retrieve the value like the above but say you wanted to get ExampleId all three tables have this field name and there could also be other field names that more than one table have with different values so this ability is taken in to account.

foreach($results as $key => $val) {
$ExampleId = $val->Example->ExampleId;
}

Basic:

Lets say you just want to get a single record from a single table though that is simple:

$record = Example::Get(1);

This will return one single object with the id 1 in this case you'll need to replace 1 with the GUID which will probably be a variable.

Stored Procedures:

to gain performance you may want to use stored procedures and there is a simple interface that was created for them.

$results = Sproc_Helper::Execute(sproc_Test_Sproc,array(Param1=>Val1,Param2=>Val2),Example);

this will use the sproc_Test_Sproc stored procedure with two parameters and return an array of Example objects.