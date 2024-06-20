template <typename T>
struct Span {
  T*  data = nullptr;
  I64 size = 0;

  Span() {
  }

  Span(T* data, I64 size) {
    this->data = data;
    this->size = size;
  }

  T* begin() {
    return data;
  }

  T* end() {
    return &data[size];
  }

  T& operator[](I64 index) {
    return data[index];
  }
};
