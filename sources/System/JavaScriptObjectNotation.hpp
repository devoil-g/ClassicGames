#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace Game
{
  class JavaScriptObjectNotation
  {
  public:
    // Forward declaration of JSON types
    class Object;
    class Array;
    class Number;
    class String;
    class Boolean;
    class Null;

    enum Type
    {
      TypeObject,   // A collection of name–value pairs
      TypeArray,    // An ordered list of zero or more elements
      TypeNumber,   // A signed decimal number
      TypeString,   // A sequence of zero or more characters
      TypeBoolean,  // Either of the values true or false
      TypeNull      // An empty value
    };

    // JSON abstract interface
    class Element
    {
    public:
      Element() = default;
      Element(const Element&) = default;
      Element(Element&&) = default;
      virtual ~Element() = 0;

      Element& operator=(const Element&) = default;
      Element& operator=(Element&&) = default;

      // Get the type of the JSON element
      virtual Type  type() const = 0;

      // Return true if null type
      virtual bool null() const;

      // Non-const accessor, throw WrongType exception when wrong type accessed
      virtual Object&      object();
      virtual Array&       array();
      virtual double&      number();
      virtual std::string& string();
      virtual bool&        boolean();

      // Const accessor, throw WrongType exception when wrong type accessed
      virtual const Object&      object() const;
      virtual const Array&       array() const;
      virtual double             number() const;
      virtual const std::string& string() const;
      virtual bool               boolean() const;

      // Write JSON to a string
      virtual std::string stringify() const = 0;
    };

    class Object : public Element
    {
    public:
      std::unordered_map<std::string, std::unique_ptr<Element>> _map; // Name-value pairs collection (NOTE: private this)

      void set(const std::string& key, std::unique_ptr<Element>&& element); // Set value at key to JSON element (NOTE: private this)

    public:
      Object() = default;
      Object(const Object&) = delete;
      Object(Object&&) = default;
      ~Object() override = default;

      Object& operator=(const Object&) = delete;
      Object& operator=(Object&&) = default;

      Type type() const override;

      Object&       object() override;
      const Object& object() const override;
      
      void set(const std::string& key, Object&& object);           // Set value at key to JSON object
      void set(const std::string& key, Array&& array);             // Set value at key to JSON array
      void set(const std::string& key, const Number& number);      // Set value at key to JSON number
      void set(const std::string& key, double number);             // Set value at key to number
      void set(const std::string& key, const String& string);      // Set value at key to JSON string
      void set(const std::string& key, String&& string);           // Set value at key to JSON string
      void set(const std::string& key, const std::string& string); // Set value at key to string
      void set(const std::string& key, std::string&& string);      // Set value at key to string
      void set(const std::string& key, const Boolean& value);      // Set value at key to JSON boolean
      void set(const std::string& key, bool value);                // Set value at key to boolean
      void set(const std::string& key);                            // Set value at key to JSON null

      Element&       get(const std::string& key);              // Get JSON element at key
      const Element& get(const std::string& key) const;        // Get JSON element at key
      Element&       operator[](const std::string& key);       // Get JSON element at key
      const Element& operator[](const std::string& key) const; // Get JSON element at key

      void        unset(const std::string& key);          // Remove element from JSON object
      bool        empty() const;                          // Check if JSON object is empty
      std::size_t size() const;                           // Get the number of entry in JSON object
      void        clear();                                // Reset JSON object
      bool        contains(const std::string& key) const; // Check if key is in JSON object

      std::string stringify() const override;
    };

    class Array : public Element
    {
    public:
      std::vector<std::unique_ptr<Element>> _vector; // Array of JSON elements (NOTE: private this)

      void push(std::unique_ptr<Element>&& element);                      // Push back a JSON element
      void set(std::size_t position, std::unique_ptr<Element>&& element); // Set a JSON element at position

    public:
      Array() = default;
      Array(const Array&) = delete;
      Array(Array&&) = default;
      ~Array() override = default;

      Array& operator=(const Array&) = delete;
      Array& operator=(Array&&) = default;

      Type type() const override;

      Array&       array() override;
      const Array& array() const override;

      void push(Object&& object);           // Push back a JSON object
      void push(Array&& array);             // Push back a JSON array
      void push(const Number& number);      // Push back a JSON number
      void push(double number);             // Push back a number
      void push(const String& string);      // Push back a JSON string
      void push(String&& string);           // Push back a JSON string
      void push(const std::string& string); // Push back a string
      void push(std::string&& string);      // Push back a string
      void push(const Boolean& value);      // Push back a JSON boolean
      void push(bool value);                // Push back a boolean
      void push();                          // Push back a JSON null

      void set(std::size_t position, Object&& object);           // Set value at position to a JSON object
      void set(std::size_t position, Array&& array);             // Set value at position to a JSON array
      void set(std::size_t position, const Number& number);      // Set value at position to a JSON number
      void set(std::size_t position, double number);             // Set value at position to a number
      void set(std::size_t position, const String& string);      // Set value at position to a JSON string
      void set(std::size_t position, String&& string);           // Set value at position to a JSON string
      void set(std::size_t position, const std::string& string); // Set value at position to a string
      void set(std::size_t position, std::string&& string);      // Set value at position to a string
      void set(std::size_t position, const Boolean& value);      // Set value at position to a JSON boolean
      void set(std::size_t position, bool value);                // Set value at position to a boolean
      void set(std::size_t position);                            // Set value at position to a JSON null

      Element&       get(std::size_t position);              // Get JSON element at position
      const Element& get(std::size_t position) const;        // Get JSON element at position
      Element&       operator[](std::size_t position);       // Get JSON element at position
      const Element& operator[](std::size_t position) const; // Get JSON element at position

      void        unset(std::size_t position); // Remove element from JSON array
      bool        empty() const;               // Check if array is empty
      std::size_t size() const;                // Get the size of the JSON array
      void        resize(std::size_t size);    // Resize the JSON array
      void        clear();                     // Reset JSON array

      std::string stringify() const override;
    };

    class Number : public Element
    {
    public:
      double value;

      Number();
      Number(double value);
      Number(const Number&) = default;
      Number(Number&&) = default;
      ~Number() override = default;

      Number& operator=(const Number&) = default;
      Number& operator=(Number&&) = default;

      Type type() const override;

      double& number() override;
      double  number() const override;
      
      std::string stringify() const override;
    };

    class String : public Element
    {
    public:
      std::string value;

      String() = default;
      String(const std::string& value);
      String(const String&) = default;
      String(std::string&& value);
      String(String&&) = default;
      ~String() override = default;

      String& operator=(const String&) = default;
      String& operator=(String&&) = default;

      Type type() const override;

      std::string&   string() override;
      const std::string& string() const override;
      
      std::string stringify() const override;
    };

    class Boolean : public Element
    {
    public:
      bool value;

      Boolean();
      Boolean(bool value);
      Boolean(const Boolean&) = default;
      Boolean(Boolean&&) = default;
      ~Boolean() override = default;

      Boolean& operator=(const Boolean&) = default;
      Boolean& operator=(Boolean&&) = default;

      Type type() const override;

      bool& boolean() override;
      bool  boolean() const override;
      
      std::string stringify() const override;
    };

    class Null : public Element
    {
    public:
      Null() = default;
      Null(const Null&) = default;
      Null(Null&&) = default;
      ~Null() override = default;

      Null& operator=(const Null&) = default;
      Null& operator=(Null&&) = default;

      Type type() const override;

      bool null() const override;
      
      std::string stringify() const override;
    };

    class TypeError : public std::runtime_error
    {
    public:
      TypeError(const std::string& message);
      ~TypeError() = default;
    };

    class BoundError : public std::runtime_error
    {
    public:
      BoundError(const std::string& message);
      ~BoundError() = default;
    };

    class FileError : public std::runtime_error
    {
    public:
      FileError(const std::string& message);
      ~FileError() = default;
    };

    class ParsingError : public std::runtime_error
    {
    public:
      ParsingError(const std::string& message);
      ~ParsingError() = default;
    };

  private:
    static void                     loadWhitespaces(const std::string& text, std::string::const_iterator& iterator);  // Skip whitespaces
    static std::unique_ptr<Element> loadElement(const std::string& text, std::string::const_iterator& iterator);      // Load JSON element

    static void loadObject(const std::string& text, std::string::const_iterator& iterator, Object& object);
    static void loadArray(const std::string& text, std::string::const_iterator& iterator, Array& array);
    static void loadNumber(const std::string& text, std::string::const_iterator& iterator, Number& number);
    static void loadString(const std::string& text, std::string::const_iterator& iterator, String& string);
    static void loadBoolean(const std::string& text, std::string::const_iterator& iterator, Boolean& boolean);
    static void loadNull(const std::string& text, std::string::const_iterator& iterator, Null& null);

    JavaScriptObjectNotation() = delete;

  public:
    static Object load(const std::filesystem::path& path);                     // Clear JSON object and load given JSON file, throw errors
    static Object load(const std::string& text);                               // Clear JSON object and load given JSON file, throw errors
    static void   save(const std::filesystem::path& path, const Object& json); // Save JSON object to file, throw error
  };

  using JSON = JavaScriptObjectNotation;
}

// Write JSON to stream
std::ostream& operator<<(std::ostream& stream, const Game::JSON::Element& json);
std::wostream& operator<<(std::wostream& stream, const Game::JSON::Element& json);