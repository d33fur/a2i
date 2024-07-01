#include "spectrogram.hpp"


void a2i::Spectrogram::setAudioInfo(
  unsigned int audio_sample_rate,
  std::pair<int, int> audio_db_range) 
{
  sample_rate = audio_sample_rate;
  db_range = audio_db_range;
}

void a2i::Spectrogram::setFreqRange(std::pair<unsigned int, unsigned int> audio_freq_range)
{
  freq_range = audio_freq_range;
}

void a2i::Spectrogram::setFrameSize(int size) 
{
  frame_size = size;
  out.resize(frame_size / 2);
  fft_out.resize(frame_size);
  window_out.resize(frame_size);
}

void a2i::Spectrogram::setWindowFunc(int type) 
{
  (this->*windows[type])();
}

void a2i::Spectrogram::windowSine()
{
  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = sin(M_PI * i / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowHann() 
{
  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = 0.5 * (1.0 - cos((2.0 * M_PI * i) / (frame_size)));
  }
}

void a2i::Spectrogram::windowHamming()
{
  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = (25. / 46) * (1.0 - cos((2.0 * M_PI * i) / (frame_size * 2)));
  }
}

void a2i::Spectrogram::windowBlackman()
{
  float alfa = 0.16;
  float a0 = (1. - alfa) / 2;
  float a1 = 1 / 2;
  float a2 = alfa / 2;

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = a0 - a1 * cos((2.0 * M_PI * i) / (frame_size * 2)) + a2 * cos((4.0 * M_PI * i) / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowNuttall()
{
  float a0 = 0.355768;
  float a1 = 0.487396;
  float a2 = 0.144232;
  float a3 = 0.012604;

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = a0 - a1 * cos((2.0 * M_PI * i) / (frame_size * 2)) + a2 * cos((4.0 * M_PI * i) / (frame_size * 2)) - a3 * cos((6.0 * M_PI * i) / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowBlackmanNuttall()
{
  float a0 = 0.3635819;
  float a1 = 0.4891775;
  float a2 = 0.1365995;
  float a3 = 0.0106411;

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = a0 - a1 * cos((2.0 * M_PI * i) / (frame_size * 2)) + a2 * cos((4.0 * M_PI * i) / (frame_size * 2)) - a3 * cos((6.0 * M_PI * i) / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowBlackmanHarris()
{
  float a0 = 0.35875;
  float a1 = 0.48829;
  float a2 = 0.14128;
  float a3 = 0.01168;

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = a0 - a1 * cos((2.0 * M_PI * i) / (frame_size * 2)) + a2 * cos((4.0 * M_PI * i) / (frame_size * 2)) - a3 * cos((6.0 * M_PI * i) / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowFlatTop()
{
  float a0 = 0.21557895;
  float a1 = 0.41663158;
  float a2 = 0.277263158;
  float a3 = 0.083578947;
  float a4 = 0.006947368;

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = a0 - a1 * cos((2.0 * M_PI * i) / (frame_size * 2)) + a2 * cos((4.0 * M_PI * i) / (frame_size * 2)) - a3 * cos((6.0 * M_PI * i) / (frame_size * 2)) + a4 * cos((8.0 * M_PI * i) / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowBartlettHann()
{
  float a0 = 0.62;
  float a1 = 0.48;
  float a2 = 0.38;

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = a0 - a1 * abs(static_cast<double>(i) / (frame_size * 2) - 1 / 2) - a2 * cos((2.0 * M_PI * i) / (frame_size * 2));
  }
}

void a2i::Spectrogram::windowHannPoisson()
{
  float a = 2; // >= 2

  for(size_t i = 0; i < static_cast<size_t>(frame_size); ++i)
  {
    window_out[i] = (1. / 2) *(1. - cos((2.0 * M_PI * i) / (frame_size * 2))) * pow(M_E, -a * abs(static_cast<double>(frame_size * 2 - 2 * i) / (frame_size * 2)));
  }
}

void a2i::Spectrogram::addWindow() 
{
  for(size_t i = 0; i < frame_size; ++i) 
  {
    in[i] *= window_out[i];
  }
}

void a2i::Spectrogram::fft()
{
  double in1[frame_size];
  fftw_complex out1[frame_size];
  fftw_plan p;

  for(size_t i = 0; i < frame_size; i++)
  {
    in1[i] = in[i];
  }

  p = fftw_plan_dft_r2c_1d(frame_size, in1, out1, FFTW_ESTIMATE);

  fftw_execute(p);

  for(size_t i = 0; i < frame_size / 2; i++)
  {
    fft_out[i] = std::complex<float>(out1[i][0], out1[i][1]);
  }

  fftw_destroy_plan(p);
  fftw_cleanup();
}

void a2i::Spectrogram::normalize(const int multiplier)
{
  for(size_t i = 0; i < frame_size / 2; ++i)
  {
    float db_value = multiplier * std::log10(std::norm(fft_out[i]) / frame_size + 1e-10);

    if (db_value < db_range.first) db_value = db_range.first + 1;
    if (db_value > db_range.second) db_value = db_range.first - 1;

    out[i] = db_value;
  }
}

void a2i::Spectrogram::drawGrid(
  cv::Mat& img, 
  const int type, 
  const bool enable_text, 
  const std::vector<unsigned int>& freqs, 
  const int number_of_db_risks,
  const cv::Scalar line_color, 
  const cv::Scalar text_color) 
{
  std::vector<unsigned int> freq_risks;

  if(freqs.size() == 0)
  {
    freq_risks = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
  }
  else
  {
    for(const auto& freq : freqs)
    {      
      freq_risks.push_back(freq);
    }
  }

  int freq_size = freq_risks.size();

  float x = 0.0f;

  cv::line(img, cv::Point(x, 0), cv::Point(x, img.rows), line_color, 1);

  if(enable_text)
  {
    std::stringstream ss;
    ss << static_cast<int>(freq_risks[0]);
    cv::putText(img, ss.str(), cv::Point(x, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 1);
  }

  for(int i = 1; i < freq_size; i++)
  {
    x = type ? 
      std::max(0., ((std::log2(freq_risks[i]) - std::log2(freq_risks[0])) / (std::log2(freq_range.second) - std::log2(freq_risks[0]))) * img.cols)
    : std::max(0., static_cast<double>(freq_risks[i] - freq_risks[0])) * img.cols * 2 / sample_rate;

    cv::line(img, cv::Point(x, 0), cv::Point(x, img.rows), line_color, 1);

    if(enable_text)
    {
      std::stringstream ss;
      ss << static_cast<int>(freq_risks[i]);
      cv::putText(img, ss.str(), cv::Point(x, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 1);
    }
  }

  for(int i = 0; i < number_of_db_risks; i++)
  {
    float db = db_range.first + i * (db_range.second - db_range.first) / number_of_db_risks;
    float y = (1 - (db - db_range.first) / (db_range.second - db_range.first)) * img.rows;

    cv::line(img, cv::Point(0, y), cv::Point(img.cols, y), line_color, 1);

    if(enable_text)
    {
      std::stringstream ss;
      ss << static_cast<int>(db);
      cv::putText(img, ss.str(), cv::Point(15, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 1);
    }
  }
}

double a2i::Spectrogram::interpolate(double from ,double to ,float percent) 
{
  double difference = to - from;
  return from + ( difference * percent );
}

void a2i::Spectrogram::drawGraientLine(
  cv::Mat& img,
  const int fill_type, 
  const cv::Point& first_point, 
  const cv::Point& second_point, 
  const cv::Scalar underline_color,
  const int gradient_color)
{
  switch(fill_type)
  {
    case NOT_FILLED :
    {
      break;
    }

    case ONE_COLOR :
    {
      cv::line(img, first_point, second_point, underline_color, 1);
      break;
    }

    case GRADIENT :
    {
      cv::line(img, first_point, second_point, cv::Scalar(gradient_color, gradient_color, gradient_color), 1);
      break;
    }

    default :
    {
      cv::line(img, first_point, second_point, underline_color, 1);
      break;
    } 
  }
}

void a2i::Spectrogram::drawSpectrum(
  cv::Mat& img, 
  const int line_type, 
  const int graph_mode, 
  const int fill_type,
  const bool border_line,
  const cv::Scalar line_color, 
  const cv::Scalar underline_color,
  const int gradient_coefficient) 
{
  int y_first;
  double x_first = 0.0;
  bool first_point = true;

  std::vector<cv::Point> control_points;
  for(size_t i = 0; i < frame_size / 2; i += 1) 
  {
    double x;
    int y;
    double freq = i * static_cast<double>(sample_rate) / frame_size;
    
    switch(graph_mode)
    {
      case LIN :
      {
        x = std::min(static_cast<double>(img.cols), 
            std::max(0., (freq - freq_range.first) * img.cols / (freq_range.second - freq_range.first)));
        break;
      }

      case LOG :
      {
        x = std::min(static_cast<double>(img.cols), 
            std::max(0., (std::log2(freq) - std::log2(freq_range.first))  * img.cols / (std::log2(freq_range.second) - std::log2(freq_range.first))));
        break;
      }

      default :
      {
        x = std::min(static_cast<double>(img.cols), 
            std::max(0., (std::log2(freq) - std::log2(freq_range.first))  * img.cols / (std::log2(freq_range.second) - std::log2(freq_range.first))));
        break;
      }
    }

    if(out[i] < 0) 
    {
      y = (1 - (std::abs(db_range.first - out[i]) / std::abs(db_range.second - db_range.first))) * img.rows;
    }
    else 
    {
      y = (1 - ((std::abs(db_range.first) + out[i]) / std::abs(db_range.second - db_range.first))) * img.rows;
    }

    if(freq >= freq_range.first && freq <= freq_range.second)
    {
      if(first_point)
      {
        control_points.push_back(cv::Point(x_first, y_first));
        first_point = false;
      }
      control_points.push_back(cv::Point(x, y));
      
    }
    else
    { 
      y_first = y;

      if(!first_point)
      {
        control_points.push_back(cv::Point(img.cols, y));
      }
    }
  }

  switch(line_type)
  {
    case BEZIE :
    {
      for(size_t i = 2; i < control_points.size() - 1; i+=3)
      {
        cv::Point p0 = control_points[i - 2];
        cv::Point p1 = control_points[i - 1];
        cv::Point p2 = control_points[i];
        cv::Point p3 = control_points[i + 1];

        double sstep = 0.6 / std::max(static_cast<double>(abs(p0.x - p3.x)), 0.0001);

        for(float j = 0; j < 1; j += sstep)
        {
          auto xa = interpolate(p0.x, p1.x ,j);
          auto ya = interpolate(p0.y, p1.y ,j);
          auto xb = interpolate(p1.x, p2.x ,j);
          auto yb = interpolate(p1.y, p2.y ,j);
          auto xc = interpolate(p2.x, p3.x ,j);
          auto yc = interpolate(p2.y, p3.y ,j);

          auto xm = interpolate(xa, xb, j);
          auto ym = interpolate(ya, yb, j);
          auto xn = interpolate(xb, xc, j);
          auto yn = interpolate(yb, yc, j);

          auto x = interpolate(xm, xn, j);
          auto y = interpolate(ym, yn, j);

          if(x >= img.cols) x = img.cols - 1;
          if(y >= img.rows) y = img.rows - 1;
          if(x <= 0) x = 1;
          if(y <= 0) y = 1;

          if(border_line)
          {
            img.at<cv::Vec3b>(cv::Point(x, y)) = cv::Vec3b(line_color[0], line_color[1], line_color[2]);
          }

          int gradient_color = static_cast<int>((1 - y / static_cast<double>(img.rows)) * gradient_coefficient);
          drawGraientLine(img, fill_type, cv::Point(x, y), cv::Point(x, img.rows), underline_color, gradient_color);
        }
      }

      break;
    }

    case LINES :
    {
      for(size_t i = 0; i < control_points.size() - 1; i+=1)
      {
        cv::Point vertices[4];
        vertices[0] = control_points[i];
        vertices[1] = control_points[i+1];
        vertices[2] = cv::Point(control_points[i+1].x, img.rows);
        vertices[3] = cv::Point(control_points[i].x, img.rows);

        for (int x = vertices[0].x; x < vertices[1].x; x++)
        {
          double t = (double)(x - vertices[0].x) / (vertices[1].x - vertices[0].x);
          int y = interpolate(vertices[0].y, vertices[1].y, t);
          int gradient_color = static_cast<int>((1 - y / static_cast<double>(img.rows)) * gradient_coefficient);
          drawGraientLine(img, fill_type, cv::Point(x, y), cv::Point(x, img.rows), underline_color, gradient_color);
        }

        if(border_line)
        {
          cv::line(img, control_points[i], control_points[i+1], line_color, 1);
        }
        

      }

      break;
    }

    case BARS :
    {
      for(size_t i = 0; i < control_points.size(); i+=1) 
      {
        int gradient_color = static_cast<int>((1 - control_points[i].y / static_cast<double>(img.rows)) * gradient_coefficient);
        drawGraientLine(img, fill_type, cv::Point(control_points[i].x, control_points[i].y), cv::Point(control_points[i].x, img.rows), underline_color, gradient_color);
      }

      break;
    }
  }
}