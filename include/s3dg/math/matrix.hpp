#pragma once
#include <array>
#include <cstdlib>
#include <spdlog/spdlog.h>


namespace s3dg {
    namespace math {
        template<size_t r_size, size_t c_size, typename T>
        class Matrix {
          private:
            std::array<T, r_size* c_size> data;
            inline size_t pos_to_ind(size_t r, size_t c) const {
                return r * c_size + c;
            }
          public:
            Matrix();
            Matrix(std::array<T, r_size* c_size> data): data(std::move(data)) {}

            Matrix<r_size, c_size, T> operator + (const Matrix<r_size, c_size, T>& other) const;
            bool operator == (const Matrix<r_size, c_size, T>& other) const;

            template<size_t oc_size>
            Matrix<r_size, oc_size, T> operator * (const Matrix<c_size, oc_size, T>& other) const;

            inline T& get() {
                return data;
            }

            inline T get(const size_t& i) const {
                return data[i];
            }

            inline T get(const size_t& r, const size_t& c) const {
                return get(pos_to_ind(r, c));
            }

            inline T operator [](const std::pair<size_t, size_t>& index) const {
                return get(index.first, index.second);
            }
            inline T operator [](const size_t& i) const {
                return get(i);
            }

            inline void set(const size_t& r, const size_t& c, const T& value) const {
                data[pos_to_ind(r, c)] = value;
            }
            inline void set(const size_t& i, const T& value) const {
                data[i] = value;
            }
            inline T& operator [](const std::pair<size_t, size_t>& index) {
                return data[pos_to_ind(index.first, index.second)];
            }
            inline T& operator [](const size_t& i) {
                return data[i];
            }

            inline void log() const;
        };

        template<size_t r_size, size_t c_size, typename T>
        Matrix<r_size, c_size, T>::Matrix() {
            for (size_t i = 0; i < r_size * c_size; ++i) {
                data[i] = 0;
            }
        }

        template<size_t r_size, size_t c_size, typename T>
        Matrix<r_size, c_size, T> Matrix<r_size, c_size, T>::operator + (const Matrix<r_size, c_size, T>& other) const {
            std::array<T, r_size* c_size> _data;

            for (size_t i = 0; i < r_size * c_size; ++i) {
                _data[i] = data[i] + other[i];
            }

            return Matrix<r_size, c_size, T>(std::move(_data));
        }

        template<size_t r_size, size_t c_size, typename T>
        bool Matrix<r_size, c_size, T>::operator == (const Matrix<r_size, c_size, T>& other) const {
            for (size_t i = 0; i < r_size * c_size; ++i)
                if (data[i] != other[i]) {
                    return false;
                }

            return true;
        }


        template<size_t r_size, size_t c_size, typename T>
        template<size_t oc_size>
        Matrix<r_size, oc_size, T> Matrix<r_size, c_size, T>::operator * (const Matrix<c_size, oc_size, T>&   other) const {
            std::array<T, r_size* oc_size> _data;

            for (size_t i = 0; i < r_size * oc_size; ++i) {
                _data[i] = 0;
            }

            for (size_t r = 0; r < r_size; ++r) {
                for (size_t oc = 0; oc < oc_size; ++oc) {
                    for (size_t c = 0; c < c_size; ++c) {
                        _data[r * oc_size + oc] += get(r, c) * other.get(c, oc);
                    }
                }
            }

            return Matrix<r_size, oc_size, T>(std::move(_data));
        }

        template<size_t r_size, size_t c_size, typename T>
        void Matrix<r_size, c_size, T>::log() const {
            std::string tmp;

            for (size_t r = 0; r < r_size; ++r) {
                tmp = "| ";

                for (size_t c = 0; c < c_size; ++c) {
                    tmp += std::to_string(get(r, c)) + " ";
                }

                tmp += "|";
                spdlog::info(tmp);
            }
        }

    }
}
