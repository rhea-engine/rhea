cmake_minimum_required (VERSION 3.9.2)

add_library(Orion STATIC Orion.cpp Realm.cpp Realms.cpp ResourceRequest.cpp ResourceCache.cpp FileStream.cpp 
"Mime.cpp" "Mime.h" "RealmRequest.h" "RealmRequest.cpp" "ResolverInterface.h" "ResolveLocal.h" "ResolveLocal.cpp"
"UrlResponseStream.h" "UrlResponseStream.cpp" "FileCollection.h" "FileCollection.cpp")
