# Code Style Guide

> Conventions and style guidelines for Convoy codebase.

## Naming Conventions

### Classes & Structs
Use **PascalCase**:
```cpp
class WindowManager { };
struct ColorState { };
class ArchitectUI { };
```

### Functions & Methods
Use **snake_case**:
```cpp
void initialize_glfw();
Color get_pixel(uint32_t x, uint32_t y);
void setup_input_commands();
```

### Variables & Members
Use **snake_case**, prefix members with `_`:
```cpp
int window_width;              // Local variable
std::string project_name_;     // Member variable
uint32_t active_layer_;        // Member variable
static int instance_count;     // Static variable
```

### Constants
Use **SCREAMING_SNAKE_CASE**:
```cpp
const int MAX_LAYERS = 32;
const float DEFAULT_OPACITY = 1.0f;
const auto WINDOW_WIDTH = 1600;
```

### Namespaces
Use **lowercase**:
```cpp
namespace convoy { }           // Main namespace
namespace architect { }        // Module namespace
namespace mod_forge { }        // Module namespace
```

### Enums
Class names PascalCase, values SCREAMING_SNAKE_CASE:
```cpp
enum class ToolType {
    PENCIL,
    ERASER,
    BUCKET,
    COLOR_PICKER
};
```

---

## File Organization

### Header Files (.h)

```cpp
#pragma once
#include <vector>
#include <string>

#include "dependency.h"

namespace convoy {

class MyClass {
   public:
    MyClass();
    ~MyClass();
    
    void public_method();
    
   private:
    void private_method();
    int private_member_;
};

}
```

**Order**:
1. `#pragma once`
2. Standard library includes
3. Third-party includes
4. Project includes
5. Namespace
6. Class definition

### Implementation Files (.cpp)

```cpp
#include "my_class.h"

#include <iostream>
#include "logger.h"

namespace convoy {

MyClass::MyClass() : private_member_(0) {}

void MyClass::public_method() {
    // Implementation
}

}
```

**Order**:
1. Own header first
2. Standard library
3. Third-party
4. Project headers
5. Namespace implementation

---

## Code Formatting

### Indentation
Use **4 spaces** (NOT tabs):
```cpp
void function() {
    if (condition) {
        do_something();
    }
}
```

### Braces
Opening brace on same line:
```cpp
// Correct
if (condition) {
    statement;
}

class MyClass {
public:
    MyClass() { }
};

// WRONG
if (condition)
{
    statement;
}
```

### Line Length
Maximum **100 characters** per line.

```cpp
// Correct
long_function_name(first_argument, second_argument, 
                   third_argument);

// Too long
very_long_function_name_that_exceeds_limit(first_arg, second_arg, third_arg, fourth_arg);
```

### Spaces

```cpp
// Around operators
int x = 5;          // Binary operators have spaces
int y = -5;         // Unary operators don't have space after
x += 10;

// After keywords
if (condition) { }
for (int i = 0; i < 10; ++i) { }
while (true) { }

// Function parameters
void function(int a, int b);  // Space after comma
function(arg1, arg2);

// No space before parenthesis in calls
function();         // Correct
function ();        // WRONG
```

---

## Comments

### Rule: Only Comment Why, Not What

**Good** - explains the "why":
```cpp
// GLFW must be initialized before creating window
glfwInit();

// Canvas size must be power of 2 for efficient GPU texture packing
if (!is_power_of_2(width)) {
    throw std::runtime_error("Width must be power of 2");
}
```

**Bad** - explains what code already says:
```cpp
// Initialize GLFW
glfwInit();

// Check if width is power of 2
if (!is_power_of_2(width)) {
    throw std::runtime_error("Width must be power of 2");
}
```

### Comment Types

**File header**: Only if file is complex
```cpp
// src/core/window_manager.cpp
// Manages GLFW window, OpenGL context, and ImGui lifecycle
// Must initialize GLAD after making context current (see TROUBLESHOOTING.md)
```

**Function header**: Only for public API
```cpp
/// Initialize Convoy window manager
/// @param title Window title
/// @param width Window width in pixels
/// @param height Window height in pixels
void initialize(const std::string& title, int width, int height);
```

**Inline comments**: Explain non-obvious logic
```cpp
// GLAD must load BEFORE ImGui tries to call OpenGL functions
if (!gladLoadGL()) {
    throw std::runtime_error("Failed to initialize GLAD");
}
```

---

## Modern C++ Practices

### Use Modern Features
```cpp
// Good - auto type deduction
auto color = Color(255, 0, 0);
for (const auto& layer : layers_) { }

// Good - range-based for
for (auto& pixel : layer.pixels) {
    pixel = 0;
}

// Good - smart pointers
std::unique_ptr<Canvas> canvas;
std::shared_ptr<Brush> brush;

// Good - move semantics
std::vector<Layer> layers;
layers.push_back(std::move(temp_layer));
```

### Avoid
```cpp
// WRONG - C-style casts
(int*)pointer;

// WRONG - C-style arrays
int array[100];

// WRONG - manual memory management
int* ptr = new int();  // Use smart pointers

// WRONG - NULL
if (ptr == NULL) { }   // Use nullptr

// WRONG - bare pointers when smart pointers work
Widget* widget = new Widget();  // Use unique_ptr
```

---

## Error Handling

### Use Exceptions
```cpp
// Good - descriptive error
if (!gladLoadGL()) {
    throw std::runtime_error("Failed to initialize GLAD");
}

// Good - custom exception
if (layers_.size() >= 32) {
    throw std::runtime_error("Maximum 32 layers allowed");
}
```

### Don't Swallow Errors
```cpp
// WRONG - silently ignores error
try {
    dangerous_operation();
} catch (...) {
    // Oops, something failed but we ignore it
}

// Correct - at least log or rethrow
try {
    dangerous_operation();
} catch (const std::exception& e) {
    Logger::error("Operation failed: {}", e.what());
    throw;  // Rethrow to caller
}
```

---

## Type Safety

### No Type Erasure
```cpp
// WRONG - never do this
Color c = my_color;
auto ptr = static_cast<void*>(&c);
auto back = static_cast<Color*>(ptr);  // Disaster waiting to happen

// WRONG - type erasure
auto value = (int)my_float;  // C-style cast

// CORRECT - explicit
int value = static_cast<int>(my_float);
```

### Const Correctness
```cpp
// Good - const method
Color get_pixel(uint32_t x, uint32_t y) const;

// Good - const reference parameter
void add_to_history(const Color& color);

// Good - const member
static const int MAX_LAYERS = 32;
```

---

## Module-Specific Guidelines

### Core Module
- Use `Logger::info()`, `Logger::error()` for output
- All public API should throw `std::runtime_error` on failure
- Prefix private members with `_`

### Architect Module
- Canvas operations should validate bounds
- Layer operations should check active layer exists
- Tool implementations should follow Tool interface

### Shared Module
- Keep types simple and POD-like
- All structs should be copyable
- No heavy dependencies

---

## Checklist Before Commit

- [ ] Names follow PascalCase (classes) / snake_case (functions)
- [ ] No `using namespace` in headers
- [ ] Comments explain "why", not "what"
- [ ] No C-style casts (`(int)x`)
- [ ] No raw `new`/`delete` (use smart pointers)
- [ ] No `nullptr` or NULL checks without reason
- [ ] Const-correct (const parameters, const methods)
- [ ] No unnecessary includes
- [ ] Member variables prefixed with `_`
- [ ] Functions under 40 lines long
- [ ] No magic numbers (use named constants)

---

## References

- [Modern C++ Best Practices](https://github.com/isocpp/cppcoreguidelines)
- C++17 Standard
- Google C++ Style Guide (partially)

