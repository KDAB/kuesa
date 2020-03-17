%modules = ( # path to module name map
    "Kuesa" => "$basedir/src/core",
    "KuesaUtils" => "$basedir/src/utils",
);

# Force generation of camel case headers for classes inside Kuesa* namespaces
$publicclassregexp = "Kuesa::.+";


