#include <codecvt>
#include <format>
#include <fstream>
#include <sstream>

#include "System/JavaScriptObjectNotation.hpp"
#include "System/Utilities.hpp"

// Default constructors
Game::JSON::Number::Number() : value(0) {}
Game::JSON::Boolean::Boolean() : value(false) {}

// Value constructors
Game::JSON::Number::Number(double value) : value(value) {}
Game::JSON::String::String(const std::wstring& value) : value(value) {}
Game::JSON::String::String(std::wstring&& value) : value(std::move(value)) {}
Game::JSON::Boolean::Boolean(bool value) : value(value) {}

// Exception constructors
Game::JSON::TypeError::TypeError(const std::string& message) : std::runtime_error(message) {}
Game::JSON::BoundError::BoundError(const std::string& message) : std::runtime_error(message) {}
Game::JSON::FileError::FileError(const std::string& message) : std::runtime_error(message) {}
Game::JSON::ParsingError::ParsingError(const std::string& message) : std::runtime_error(message) {}

// Destructors
Game::JSON::Element::~Element() = default;

// Type getters
Game::JSON::Type Game::JSON::Object::type() const { return Game::JSON::Type::Object; }
Game::JSON::Type Game::JSON::Array::type() const { return Game::JSON::Type::Array; }
Game::JSON::Type Game::JSON::Number::type() const { return Game::JSON::Type::Number; }
Game::JSON::Type Game::JSON::String::type() const { return Game::JSON::Type::String; }
Game::JSON::Type Game::JSON::Boolean::type() const { return Game::JSON::Type::Boolean; }
Game::JSON::Type Game::JSON::Null::type() const { return Game::JSON::Type::Null; }

// Default accessors to error
Game::JSON::Object& Game::JSON::Element::object() { throw Game::JSON::TypeError("JSON Element is not a JSON Object"); }
Game::JSON::Array& Game::JSON::Element::array() { throw Game::JSON::TypeError("JSON Element is not a JSON Array"); }
double& Game::JSON::Element::number() { throw Game::JSON::TypeError("JSON Element is not a JSON Number"); }
std::wstring& Game::JSON::Element::string() { throw Game::JSON::TypeError("JSON Element is not a JSON String"); }
bool& Game::JSON::Element::boolean() { throw Game::JSON::TypeError("JSON Element is not a JSON Boolean"); }

// Default const accessors to error
const Game::JSON::Object& Game::JSON::Element::object() const { throw Game::JSON::TypeError("JSON Element is not a JSON Object"); }
const Game::JSON::Array& Game::JSON::Element::array() const { throw Game::JSON::TypeError("JSON Element is not a JSON Array"); }
double Game::JSON::Element::number() const { throw Game::JSON::TypeError("JSON Element is not a JSON Number"); }
const std::wstring& Game::JSON::Element::string() const { throw Game::JSON::TypeError("JSON Element is not a JSON String"); }
bool Game::JSON::Element::boolean() const { throw Game::JSON::TypeError("JSON Element is not a JSON Boolean"); }

// Define accessors
Game::JSON::Object& Game::JSON::Object::object() { return *this; }
Game::JSON::Array& Game::JSON::Array::array() { return *this; }
double& Game::JSON::Number::number() { return this->value; }
std::wstring& Game::JSON::String::string() { return this->value; }
bool& Game::JSON::Boolean::boolean() { return this->value; }

// Define const accessors
const Game::JSON::Object& Game::JSON::Object::object() const { return *this; }
const Game::JSON::Array& Game::JSON::Array::array() const { return *this; }
double Game::JSON::Number::number() const { return this->value; }
const std::wstring& Game::JSON::String::string() const { return this->value; }
bool Game::JSON::Boolean::boolean() const { return this->value; }

// Object element setters
void  Game::JSON::Object::set(const std::wstring& key, Game::JSON::Object&& object) { set(key, std::make_unique<Game::JSON::Object>(std::move(object))); }
void  Game::JSON::Object::set(const std::wstring& key, Game::JSON::Array&& array) { set(key, std::make_unique<Game::JSON::Array>(std::move(array))); }
void  Game::JSON::Object::set(const std::wstring& key, const Game::JSON::Number& number) { set(key, std::make_unique<Game::JSON::Number>(number)); }
void  Game::JSON::Object::set(const std::wstring& key, double number) { set(key, std::make_unique<Game::JSON::Number>(number)); }
void  Game::JSON::Object::set(const std::wstring& key, const Game::JSON::String& string) { set(key, std::make_unique<Game::JSON::String>(string)); }
void  Game::JSON::Object::set(const std::wstring& key, Game::JSON::String&& string) { set(key, std::make_unique<Game::JSON::String>(std::move(string))); }
void  Game::JSON::Object::set(const std::wstring& key, const std::wstring& string) { set(key, std::make_unique<Game::JSON::String>(string)); }
void  Game::JSON::Object::set(const std::wstring& key, std::wstring&& string) { set(key, std::make_unique<Game::JSON::String>(std::move(string))); }
void  Game::JSON::Object::set(const std::wstring& key, const Game::JSON::Boolean& value) { set(key, std::make_unique<Game::JSON::Boolean>(value)); }
void  Game::JSON::Object::set(const std::wstring& key, bool value) { set(key, std::make_unique<Game::JSON::Boolean>(value)); }
void  Game::JSON::Object::set(const std::wstring& key) { set(key, std::make_unique<Game::JSON::Null>()); }

// Array element pusher
void  Game::JSON::Array::push(Game::JSON::Object&& object) { _vector.push_back(std::make_unique<Game::JSON::Object>(std::move(object))); }
void  Game::JSON::Array::push(Game::JSON::Array&& array) { _vector.push_back(std::make_unique<Game::JSON::Array>(std::move(array))); }
void  Game::JSON::Array::push(const Game::JSON::Number& number) { _vector.push_back(std::make_unique<Game::JSON::Number>(number)); }
void  Game::JSON::Array::push(double number) { _vector.push_back(std::make_unique<Game::JSON::Number>(number)); }
void  Game::JSON::Array::push(const Game::JSON::String& string) { _vector.push_back(std::make_unique<Game::JSON::String>(string)); }
void  Game::JSON::Array::push(Game::JSON::String&& string) { _vector.push_back(std::make_unique<Game::JSON::String>(std::move(string))); }
void  Game::JSON::Array::push(const std::wstring& string) { _vector.push_back(std::make_unique<Game::JSON::String>(string)); }
void  Game::JSON::Array::push(std::wstring&& string) { _vector.push_back(std::make_unique<Game::JSON::String>(std::move(string))); }
void  Game::JSON::Array::push(const Game::JSON::Boolean& value) { _vector.push_back(std::make_unique<Game::JSON::Boolean>(value)); }
void  Game::JSON::Array::push(bool value) { _vector.push_back(std::make_unique<Game::JSON::Boolean>(value)); }
void  Game::JSON::Array::push() { _vector.push_back(std::make_unique<Game::JSON::Null>()); }

// Array element setter
void  Game::JSON::Array::set(std::size_t position, Game::JSON::Object&& object) { set(position, std::make_unique<Game::JSON::Object>(std::move(object))); }
void  Game::JSON::Array::set(std::size_t position, Game::JSON::Array&& array) { set(position, std::make_unique<Game::JSON::Array>(std::move(array))); }
void  Game::JSON::Array::set(std::size_t position, const Game::JSON::Number& number) { set(position, std::make_unique<Game::JSON::Number>(number)); }
void  Game::JSON::Array::set(std::size_t position, double number) { set(position, std::make_unique<Game::JSON::Number>(number)); }
void  Game::JSON::Array::set(std::size_t position, const Game::JSON::String& string) { set(position, std::make_unique<Game::JSON::String>(string)); }
void  Game::JSON::Array::set(std::size_t position, Game::JSON::String&& string) { set(position, std::make_unique<Game::JSON::String>(std::move(string))); }
void  Game::JSON::Array::set(std::size_t position, const std::wstring& string) { set(position, std::make_unique<Game::JSON::String>(string)); }
void  Game::JSON::Array::set(std::size_t position, std::wstring&& string) { set(position, std::make_unique<Game::JSON::String>(std::move(string))); }
void  Game::JSON::Array::set(std::size_t position, const Game::JSON::Boolean& value) { set(position, std::make_unique<Game::JSON::Boolean>(value)); }
void  Game::JSON::Array::set(std::size_t position, bool value) { set(position, std::make_unique<Game::JSON::Boolean>(value)); }
void  Game::JSON::Array::set(std::size_t position) { set(position, std::make_unique<Game::JSON::Null>()); }

// Null element
bool Game::JSON::Element::null() const { return false; }
bool Game::JSON::Null::null() const { return true; }

void  Game::JSON::Object::set(const std::wstring& key, std::unique_ptr<Game::JSON::Element>&& element)
{
  // Add element in container at key
  _map[key] = std::move(element);
}

Game::JSON::Element& Game::JSON::Object::get(const std::wstring& key)
{
  // Check key exists
  return *_map.at(key);
}

const Game::JSON::Element& Game::JSON::Object::get(const std::wstring& key) const
{
  // Check key exists
  return *_map.at(key);
}

Game::JSON::Element& Game::JSON::Object::operator[](const std::wstring& key)
{
  // Get element at key
  return get(key);
}

const Game::JSON::Element& Game::JSON::Object::operator[](const std::wstring& key) const
{
  // Get element at key
  return get(key);
}

void  Game::JSON::Object::unset(const std::wstring& key)
{
  // Remove element from map
  _map.erase(key);
}

bool  Game::JSON::Object::empty() const
{
  // Check if map is empty
  return _map.empty();
}

std::size_t Game::JSON::Object::size() const
{
  // Get number of elements in map
  return _map.size();
}

void  Game::JSON::Object::clear()
{
  // Reset container
  _map.clear();
}

bool  Game::JSON::Object::contains(const std::wstring& key) const
{
  // Find key in container
  return _map.contains(key);
}

Game::JSON::Object::iterator  Game::JSON::Object::begin()
{
  // Returns begin iterator
  return _map.begin();
}

Game::JSON::Object::const_iterator  Game::JSON::Object::begin() const
{
  // Returns const begin iterator
  return _map.begin();
}

Game::JSON::Object::iterator  Game::JSON::Object::end()
{
  // Returns end iterator
  return _map.end();
}

Game::JSON::Object::const_iterator  Game::JSON::Object::end() const
{
  // Returns const end iterator
  return _map.end();
}

Game::JSON::Object::Object(const std::filesystem::path& path) :
  _map()
{
  std::wifstream  file(path);
  
  // Failed to load file
  if (file.fail() == true)
    throw Game::JSON::FileError(std::string("Failed to open '") + path.string() + std::string("'"));

  // Set UTF-8 to wchar
  file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

  std::wstringstream stream;
  
  // Read file
  stream << file.rdbuf();

  // Load JSON from string
  load(stream.str());
}

Game::JSON::Object::Object(const std::wstring& text) :
  _map()
{
  // Load JSON from string
  load(text);
}

void  Game::JSON::Object::load(const std::wstring& text)
{
  Game::JSON::Object  object;
  auto                iterator = text.begin();

  // Load object
  loadObject(text, iterator, object);

  // Skip last whitespaces
  loadWhitespaces(text, iterator);

  // Iterator must be at the end of the file now
  if (iterator != text.end())
    throw Game::JSON::ParsingError("Trailing characters (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Move loaded object to instance
  *this = std::move(object);
}

void  Game::JSON::Object::loadWhitespaces(const std::wstring& text, std::wstring::const_iterator& iterator)
{
  // Skip whitespaces
  while (iterator != text.end() && std::wstring_view(L" \t\n\r").find_first_of(*iterator) != std::string_view::npos)
    iterator++;
}

std::unique_ptr<Game::JSON::Element>  Game::JSON::Object::loadElement(const std::wstring& text, std::wstring::const_iterator& iterator)
{
  // Skip whitespaces
  loadWhitespaces(text, iterator);

  // End of file
  if (iterator == text.end())
    throw Game::JSON::ParsingError("End of file in element (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // JSON Object
  if (*iterator == '{') {
    Game::JSON::Object  object;

    loadObject(text, iterator, object);
    return std::make_unique<Game::JSON::Object>(std::move(object));
  }

  // JSON Array
  if (*iterator == '[') {
    Game::JSON::Array   array;

    loadArray(text, iterator, array);
    return std::make_unique<Game::JSON::Array>(std::move(array));
  }

  // JSON Number
  if (*iterator == '+' || *iterator == '-' || (*iterator >= '0' && *iterator <= '9')) {
    Game::JSON::Number  number;

    loadNumber(text, iterator, number);
    return std::make_unique<Game::JSON::Number>(std::move(number));
  }

  // JSON String
  if (*iterator == '"') {
    Game::JSON::String  string;

    loadString(text, iterator, string);
    return std::make_unique<Game::JSON::String>(std::move(string));
  }

  // JSON Boolean
  if (*iterator == 't' || *iterator == 'f') {
    Game::JSON::Boolean boolean;

    loadBoolean(text, iterator, boolean);
    return std::make_unique<Game::JSON::Boolean>(std::move(boolean));
  }

  // JSON Null
  if (*iterator == 'n') {
    Game::JSON::Null    null;

    loadNull(text, iterator, null);
    return std::make_unique<Game::JSON::Null>(std::move(null));
  }

  // No match
  throw Game::JSON::ParsingError("Unexpected character (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");
}

void  Game::JSON::Object::loadObject(const std::wstring& text, std::wstring::const_iterator& iterator, Game::JSON::Object& object)
{
  // Opening curly bracket
  loadWhitespaces(text, iterator);
  if (iterator == text.end() || *iterator != '{')
    throw Game::JSON::ParsingError("Expected opening curly bracket (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip opening curly bracket
  iterator++;

  while (true)
  {
    // Skip whitespaces
    loadWhitespaces(text, iterator);

    // End of file
    if (iterator == text.end())
      throw Game::JSON::ParsingError("End of file in object (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

    // End of object
    if (*iterator == '}')
      break;

    Game::JSON::String  name;

    // Load item name
    loadString(text, iterator, name);

    // Item name already registered
    if (object.contains(name.value) == true)
      throw Game::JSON::ParsingError("Name already registerd in object: \"" + Game::Utilities::Convert(name.value) + "\" (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

    // Check name-value separator
    loadWhitespaces(text, iterator);
    if (iterator == text.end() || *iterator != ':')
      throw Game::JSON::ParsingError("Expected name-value separator (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

    // Skip name-value separator
    iterator++;

    // Insert name-value in map
    object.set(name.value, loadElement(text, iterator));

    // Skip whitespaces
    loadWhitespaces(text, iterator);

    // End of file
    if (iterator == text.end())
      throw Game::JSON::ParsingError("End of file in object (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

    // Skip separator
    if (*iterator != ',')
      break;

    // Skip separator
    iterator++;
  }

  // Closing curly bracket
  loadWhitespaces(text, iterator);
  if (iterator == text.end() || *iterator != '}')
    throw Game::JSON::ParsingError("Expected closing curly bracket (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip closing curly bracket
  iterator++;
}

void  Game::JSON::Object::loadArray(const std::wstring& text, std::wstring::const_iterator& iterator, Game::JSON::Array& array)
{
  // Opening bracket
  loadWhitespaces(text, iterator);
  if (iterator == text.end() || *iterator != '[')
    throw Game::JSON::ParsingError("Expected opening bracket (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip opening bracket
  iterator++;

  while (true)
  {
    // Skip whitespaces
    loadWhitespaces(text, iterator);

    // End of file
    if (iterator == text.end())
      throw Game::JSON::ParsingError("End of file in array (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

    // End of array
    if (*iterator == ']')
      break;

    // Insert value in vector
    array.push(loadElement(text, iterator));

    // Skip whitespaces
    loadWhitespaces(text, iterator);

    // End of file
    if (iterator == text.end())
      throw Game::JSON::ParsingError("End of file in array (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

    // Skip separator
    if (*iterator != ',')
      break;

    // Skip separator
    iterator++;
  }

  // Closing bracket
  loadWhitespaces(text, iterator);
  if (iterator == text.end() || *iterator != ']')
    throw Game::JSON::ParsingError("Expected closing bracket (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip closing bracket
  iterator++;
}

void  Game::JSON::Object::loadNumber(const std::wstring& text, std::wstring::const_iterator& iterator, Game::JSON::Number& number)
{
  // Skip whitespaces
  loadWhitespaces(text, iterator);

  std::size_t pos = 0;
  
  // Read double from string
  // TODO: support 0b binary, 0 octal & 0x hex prefix
  number.value = std::stod(std::wstring(iterator, text.end()), &pos);

  // No character read from string
  if (pos == 0)
    throw Game::JSON::ParsingError("Expected number (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip number characters
  std::advance(iterator, pos);
}

void  Game::JSON::Object::loadString(const std::wstring& text, std::wstring::const_iterator& iterator, Game::JSON::String& string)
{
  // Double quotes
  loadWhitespaces(text, iterator);
  if (iterator == text.end() || *iterator != '"')
    throw Game::JSON::ParsingError("Expected double quote (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip double quote
  iterator++;

  while (true)
  {
    // End of file
    if (iterator == text.end())
      throw Game::JSON::ParsingError("End of file in string (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");
    
    // End of string
    if (*iterator == '"')
      break;

    wchar_t c;

    // Escaping character
    if (*iterator == '\\') {
      iterator++;

      // No more characters
      if (iterator == text.end())
        throw Game::JSON::ParsingError("End of file in string (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

      // Select escaped character
      switch (*iterator) {
      case '0': c = '\0'; break;
      case 'a': c = '\a'; break;
      case 'b': c = '\b'; break;
      case 't': c = '\t'; break;
      case 'n': c = '\n'; break;
      case 'v': c = '\v'; break;
      case 'f': c = '\f'; break;
      case 'r': c = '\r'; break;
      default: c = *iterator; break;
      }
    }

    // Simple character
    else
      c = *iterator;

    // Push character to string
    string.value += c;

    // Next character
    iterator++;
  }

  // Double quotes
  loadWhitespaces(text, iterator);
  if (iterator == text.end() || *iterator != '"')
    throw Game::JSON::ParsingError("Expected double quote (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");

  // Skip double quote
  iterator++;
}

void  Game::JSON::Object::loadBoolean(const std::wstring& text, std::wstring::const_iterator& iterator, Game::JSON::Boolean& boolean)
{
  // Skip whitespaces
  loadWhitespaces(text, iterator);

  // True value
  if (std::wstring(iterator, text.end()).substr(0, 4) == L"true") {
    boolean.value = true;
    std::advance(iterator, 4);
  }

  // False value
  else if (std::wstring(iterator, text.end()).substr(0, 5) == L"false") {
    boolean.value = false;
    std::advance(iterator, 5);
  }

  // Invalid value
  else
    throw Game::JSON::ParsingError("Expected boolean value (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");
}

void  Game::JSON::Object::loadNull(const std::wstring& text, std::wstring::const_iterator& iterator, Game::JSON::Null& null)
{
  // Skip whitespaces
  loadWhitespaces(text, iterator);

  // Null value
  if (std::wstring(iterator, text.end()).substr(0, 4) == L"null")
    std::advance(iterator, 4);

  // Invalid value
  else
    throw Game::JSON::ParsingError("Expected boolean value (pos: " + std::to_string(std::distance(text.begin(), iterator)) + ")");
}

void  Game::JSON::Array::push(std::unique_ptr<Game::JSON::Element>&& element)
{
  // Push element
  _vector.emplace_back(std::move(element));
}

void  Game::JSON::Array::set(std::size_t position, std::unique_ptr<Game::JSON::Element>&& element)
{
  // Set element
  _vector.at(position) = std::move(element);
}

Game::JSON::Element& Game::JSON::Array::get(std::size_t position)
{
  // Get element at position
  return *_vector.at(position);
}

const Game::JSON::Element& Game::JSON::Array::get(std::size_t position) const
{
  // Get element at position
  return *_vector.at(position);
}

Game::JSON::Element& Game::JSON::Array::operator[](std::size_t position)
{
  // Get element at position
  return get(position);
}

const Game::JSON::Element& Game::JSON::Array::operator[](std::size_t position) const
{
  // Get element at position
  return get(position);
}

void  Game::JSON::Array::unset(std::size_t position)
{
  // Remove element from container
  _vector.erase(_vector.begin() + position);
}

bool  Game::JSON::Array::empty() const
{
  // Check that vector is empty
  return _vector.empty();
}

std::size_t Game::JSON::Array::size() const
{
  // Get size of the array
  return _vector.size();
}

void  Game::JSON::Array::resize(std::size_t size)
{
  // Grow container with JSON Null
  if (this->size() < size) {
    _vector.reserve(size);
    while (this->size() < size)
      _vector.emplace_back(std::make_unique<Game::JSON::Null>());
  }

  // Remove elements from container
  else if (this->size() > size)
    _vector.erase(_vector.begin() + size, _vector.end());
}

void  Game::JSON::Array::reserve(std::size_t size)
{
  // Pre-allocate JSON array
  _vector.reserve(size);
}

void  Game::JSON::Array::clear()
{
  // Reset container
  _vector.clear();
}

Game::JSON::Array::iterator Game::JSON::Array::begin()
{
  // Returns begin iterator
  return _vector.begin();
}

Game::JSON::Array::const_iterator Game::JSON::Array::begin() const
{
  // Returns const begin iterator
  return _vector.begin();
}

Game::JSON::Array::iterator Game::JSON::Array::end()
{
  // Returns end iterator
  return _vector.end();
}

Game::JSON::Array::const_iterator Game::JSON::Array::end() const
{
  // Returns const end iterator
  return _vector.end();
}

std::wstring Game::JSON::Object::stringify() const
{
  std::wostringstream stream;

  // Opening curly bracket
  stream << L"{";

  // Dump each element
  for (auto it = _map.begin(); it != _map.end(); it++)
    stream << (it == _map.begin() ? L"" : L",") << L"\"" << it->first << L"\":" << *it->second;

  // Closing curly bracket
  stream << L"}";

  return stream.str();
}

std::wstring Game::JSON::Array::stringify() const
{
  std::wostringstream  stream;

  // Opening bracket
  stream << L"[";

  // Dump each element
  for (auto it = _vector.begin(); it != _vector.end(); it++)
    stream << (it == _vector.begin() ? "" : ",") << **it;

  // Closing bracket
  stream << L"]";

  return stream.str();
}

std::wstring Game::JSON::Number::stringify() const
{
  // Shortest decimal representation with a round-trip guarantee
  return Game::Utilities::Convert(std::format("{}", value));
}

std::wstring Game::JSON::String::stringify() const
{
  // Text between quotes
  return std::wstring(L"\"") + value + std::wstring(L"\"");
}

std::wstring Game::JSON::Boolean::stringify() const
{
  // Bool to text
  return value == true ? L"true" : L"false";
}

std::wstring Game::JSON::Null::stringify() const
{
  // Simple string
  return L"null";
}

std::wostream& operator<<(std::wostream& stream, const Game::JSON::Element& json)
{
  // Just dump JSON to stream
  return stream << json.stringify();
}