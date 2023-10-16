#include <WString.h>

String JSON_ArrayStart() {
  return "{\"objects\":[";
}
String JSON_ArrayDivider() {
  return ",";
}
String JSON_ArrayEnd() {
  return "]}";
}
String JSON_item(String Name, String Value) {
  return "{\"item\":\""+Name+"\",\"value\":\""+Value+"\"}";
}
