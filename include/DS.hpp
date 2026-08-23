#pragma once
#include "utility.hpp"
#include <cmath>
#include <vector>

namespace Koral {
namespace DS {
  template <typename T = float>
  class Vector {
  public:
    Vector() = default;
    Vector(size_t size, T init = 0.0f) : data_(size, init) {}
    Vector(std::initializer_list<T> vals) : data_(vals) {}
    
    template <typename Distribution>
    auto Rand(Distribution dist) {
      for (auto& x : data_) x = dist(gen);
      return *this;
    }
    
    auto relu() {
      for (size_t i = 0; i < data_.size(); ++i) {
        for (auto& element : data_) {
          element = (element > static_cast<T>(0)) ? element : static_cast<T>(0);
        }
      }
      return *this;
    }

    [[nodiscard]] size_t size() const noexcept { return data_.size(); }
    [[nodiscard]] double& operator[](size_t i) noexcept { return data_[i]; }
    [[nodiscard]] const double& operator[](size_t i) const noexcept { return data_[i]; }
    [[nodiscard]] std::span<const T> span() const noexcept {
  return std::span<const T>(data_.data(), data_.size());
}

  private:
    std::vector<T> data_;
  };
  
  template <typename T = float>
  class Matrix {
  public:
    Matrix() = default;
    Matrix(size_t rows, size_t cols, T init = static_cast<T>(0))
      : rows_(rows), cols_(cols), data_(rows * cols, init) {}

    [[nodiscard]] size_t rows() const noexcept { return rows_; }
    [[nodiscard]] size_t cols() const noexcept { return cols_; }

    [[nodiscard]] T& operator()(size_t r, size_t c) noexcept { return data_[r * cols_ + c]; }
    [[nodiscard]] const T& operator()(size_t r, size_t c) const noexcept { return data_[r * cols_ + c]; }

    template <typename Distribution>
    auto Rand(Distribution dist) {
      for (auto& x : data_) x = dist(gen);
      return *this;
    }
    
    
    auto relu() {
      for (size_t i = 0; i < data_.size(); ++i) {
        for (auto& element : data_) {
          element = (element > static_cast<T>(0)) ? element : static_cast<T>(0);
        }
      }
      return *this;
    }

    [[nodiscard]] std::span<T> row(size_t r) noexcept {
      return std::span<T>(data_.data() + r * cols_, cols_);
    }
    [[nodiscard]] std::span<const T> row(size_t r) const noexcept {
      return std::span<const T>(data_.data() + r * cols_, cols_);
    }

  private:
    size_t rows_ = 0, cols_ = 0;
    std::vector<T> data_;
  };

  namespace algo {
    template<typename T>
[[nodiscard]] auto vecLen(std::span< const T> span1) noexcept {
  return std::sqrt(DS::algo::scmul(span1, span1));
}
    template<typename T>
[[nodiscard]] auto normalize(std::span<const T> span1) {
  T len = vecLen(span1);
  std::vector<T> result(span1.size());
  for (size_t i = 0; i < span1.size(); ++i) result[i] = span1[i] / len;
  return result;
}
    template <typename T>
    [[nodiscard]] auto scmul(std::span<const T> a, std::span<const T> b) noexcept -> T {
      T result = static_cast<T>(0);
      size_t size = std::min(a.size(), b.size());
      for (size_t i = 0; i < size; ++i) {
        result += a[i] * b[i];
      }
      return result;
    }

    template <typename T>
    [[nodiscard]] auto sigmoid(T num, T limit = static_cast<T>(1)) noexcept -> T {
      return limit / (static_cast<T>(1) + std::exp(-num));
    }

    template <typename T>
    [[nodiscard]] auto relu(T num) noexcept -> T {
      return (num > static_cast<T>(0)) ? num : static_cast<T>(0);
    }

    template <typename T>
    void sigmoid(std::span<T> vec, T limit = static_cast<T>(1)) noexcept {
      for (auto& element : vec) {
        element = DS::algo::sigmoid(element, limit);
      }
    }

    template <typename T>
    void relu(std::span<T> vec) noexcept {
      for (auto& element : vec) {
        element = DS::algo::relu(element);
      }
    }

    template <typename T>
    [[nodiscard]] auto mse(std::span<const T> actual, std::span<const T> pred) noexcept -> T {
      T sum = static_cast<T>(0);
      size_t size = std::min(actual.size(), pred.size());
      for (size_t i = 0; i < size; ++i) {
        T diff = actual[i] - pred[i];
        sum += diff * diff;
      }
      return sum / static_cast<T>(size);
    }

    template <typename T>
    [[nodiscard]] auto autoFit(std::span<const T> inputs, T target, std::span<const T> weights, T limit = static_cast<T>(100)) -> T {
      
    }

    namespace upgrade {
      template <typename T>
      void fit(std::span<const T> inputs, T target, T speed, size_t epochs, std::span<T> weights) {
        T limit = static_cast<T>(100);
        size_t size = std::min(inputs.size(), weights.size());

        for (size_t e = 0; e < epochs; ++e) {
          T score = DS::algo::scmul<T>(inputs, weights);
          T prob = DS::algo::sigmoid(score, limit);
          T error = target - prob;

          if (std::abs(error) < static_cast<T>(1e-4)) break;

          T norm_prob = prob / limit;
          T delta = error * (norm_prob * (static_cast<T>(1) - norm_prob));

          for (size_t i = 0; i < size; ++i) {
            weights[i] += speed * delta * inputs[i];
          }
        }
      }

      template <typename T>
      void weights(std::span<const T> inputs, T target, T speed, std::span<T> weights, T lim = static_cast<T>(100)) {
        T limit = lim;
        T score = DS::algo::scmul<T>(inputs, weights);
        T prob = DS::algo::sigmoid(score, limit);

        T error = target - prob;
        if (std::abs(error) <= static_cast<T>(1e-5)) return;

        T norm_prob = prob / limit;
        T delta = error * (norm_prob * (static_cast<T>(1) - norm_prob));

        size_t size = std::min(inputs.size(), weights.size());
        for (size_t i = 0; i < size; ++i) {
          weights[i] += speed * delta * inputs[i];
        }
      }
    }

    namespace layer {
      template <typename T>
      [[nodiscard]] auto forward(std::span<const T> inputs, const DS::Matrix<T>& weights, T limit = static_cast<T>(100)) -> std::vector<T> {
        std::vector<T> outputs(weights.rows());
        for (size_t n = 0; n < weights.rows(); ++n) {
          T score = DS::algo::scmul<T>(inputs, weights.row(n));
          outputs[n] = DS::algo::sigmoid(score, limit);
        }
        return outputs;
      }

      namespace upgrade {

        template <typename T>
        void autoFeed(std::span<const T> inputs, T target, DS::Matrix<T>& weights, T limit = static_cast<T>(100)) {
          for (size_t n = 0; n < weights.rows(); ++n) {
            auto row = weights.row(n);
            T speed = DS::algo::autoFit<T>(inputs, target, row, limit);
            if (std::isnan(speed)) continue;
            DS::algo::upgrade::weights<T>(inputs, target, speed, row, limit);
          }
        }
      }
    }
  }

  namespace io {
    namespace detail {
      template <typename T>
      [[nodiscard]] bool wovec(const std::string& path, std::span<const T> data) {
        std::ofstream out(path, std::ios::trunc);
        if (!out.is_open()) return false;
        out.precision(std::numeric_limits<T>::max_digits10);
        for (const T& v : data) {
          out << v << '\n';
        }
        return static_cast<bool>(out);
      }

      template <typename T>
      [[nodiscard]] bool rovec(const std::string& path, std::vector<T>& data) {
        std::ifstream in(path);
        if (!in.is_open()) return false;
        data.clear();
        T value;
        while (in >> value) {
          data.push_back(value);
        }
        return true;
      }
    }

    template <typename T>
    [[nodiscard]] bool save(std::string weights_file, std::string inputs_file, std::span<const T> weights, std::span<const T> inputs) {
      std::error_code ec;
      std::filesystem::create_directories("saves", ec);
      if (ec) {
        println("Error: can't create 'saves' directory");
        return false;
      }

      std::string weights_path = "saves/" + std::move(weights_file);
      std::string inputs_path  = "saves/" + std::move(inputs_file);

      bool ok_w = detail::wovec<T>(weights_path, weights);
      bool ok_i = detail::wovec<T>(inputs_path, inputs);

      if (!ok_w || !ok_i) {
        println("Error: failed to save one or both files");
        return false;
      }
      println("Saved successfully!");
      return true;
    }

    template <typename T = double>
    [[nodiscard]] std::pair<std::vector<T>, std::vector<T>> load(std::string weights_file, std::string inputs_file) {
      std::string weights_path = "saves/" + std::move(weights_file);
      std::string inputs_path  = "saves/" + std::move(inputs_file);

      std::vector<T> w_data;
      std::vector<T> i_data;

      bool ok_w = detail::rovec<T>(weights_path, w_data);
      bool ok_i = detail::rovec<T>(inputs_path, i_data);

      if (!ok_w || !ok_i) {
        println("Error: file not found or corrupted");
        return std::make_pair(std::vector<T>{}, std::vector<T>{});
      }
      println("Loaded successfully!");
      return std::make_pair(std::move(w_data), std::move(i_data));
    }
  }
}}