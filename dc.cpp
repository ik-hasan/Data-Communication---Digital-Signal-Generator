#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <cstdlib>

using namespace std;

// LONGEST PALINDROME - Manacher's Algorithm
string findLongestPalindrome(const string &data)
{
  if (data.empty())
    return "";

  string modified = "#";
  for (char c : data)
  {
    modified += c;
    modified += "#";
  }

  int n = modified.length();
  vector<int> P(n, 0);
  int center = 0, right = 0;

  for (int i = 0; i < n; i++)
  {
    int mirror = 2 * center - i;
    if (i < right)
      P[i] = min(right - i, P[mirror]);

    int a = i + (1 + P[i]);
    int b = i - (1 + P[i]);
    while (a < n && b >= 0 && modified[a] == modified[b])
    {
      P[i]++;
      a++;
      b--;
    }

    if (i + P[i] > right)
    {
      center = i;
      right = i + P[i];
    }
  }

  int maxLen = 0, centerIdx = 0;
  for (int i = 0; i < n; i++)
    if (P[i] > maxLen)
    {
      maxLen = P[i];
      centerIdx = i;
    }

  int start = (centerIdx - maxLen) / 2;
  return data.substr(start, maxLen);
}

// ENCODERS

vector<int> nrzl_encode(const string &data)
{
  vector<int> signal;
  for (char bit : data)
    signal.push_back(bit == '1' ? -1 : 1);
  return signal;
}

vector<int> nrzi_encode(const string &data)
{
  vector<int> signal;
  int current = -1;
  for (char bit : data)
  {
    if (bit == '1')
      current = -current;
    signal.push_back(-current);
  }
  return signal;
}

vector<int> manchester_encode(const string &data)
{
  vector<int> signal;
  for (char bit : data)
  {
    if (bit == '1')
    {
      signal.push_back(-1);
      signal.push_back(1);
    }
    else
    {
      signal.push_back(1);
      signal.push_back(-1);
    }
  }
  return signal;
}

vector<int> diff_manchester_encode(const string &data)
{
  vector<int> signal;
  int current = 1;
  for (char bit : data)
  {
    if (bit == '0')
      current = -current;
    if (current == 1)
    {
      signal.push_back(1);
      signal.push_back(-1);
    }
    else
    {
      signal.push_back(-1);
      signal.push_back(1);
    }
    current = -current;
  }
  return signal;
}

vector<int> ami_encode(const string &data)
{
  vector<int> signal;
  int lastPulse = 1;
  for (char bit : data)
  {
    if (bit == '1')
    {
      signal.push_back(lastPulse);
      lastPulse = -lastPulse;
    }
    else
      signal.push_back(0);
  }
  return signal;
}

vector<int> b8zs_scramble(const string &data)
{
  vector<int> signal;
  int lastPulse = 1;

  for (size_t i = 0; i < data.length();)
  {
    if (i + 8 <= data.length() && data.substr(i, 8) == "00000000")
    {
      int prevPolarity = 1;
      for (int j = signal.size() - 1; j >= 0; --j)
        if (signal[j] != 0)
        {
          prevPolarity = signal[j];
          break;
        }

      if (prevPolarity == 1)
        signal.insert(signal.end(), {0, 0, 0, 1, -1, 0, -1, 1});
      else
        signal.insert(signal.end(), {0, 0, 0, -1, 1, 0, 1, -1});
      i += 8;
    }
    else
    {
      if (data[i] == '1')
      {
        signal.push_back(lastPulse);
        lastPulse = -lastPulse;
      }
      else
        signal.push_back(0);
      i++;
    }
  }
  return signal;
}

vector<int> hdb3_scramble(const string &data)
{
  vector<int> signal;
  int lastPulse = -1;
  int pulseCount = 0;

  for (size_t i = 0; i < data.length();)
  {
    if (i + 3 < data.length() && data.substr(i, 4) == "0000")
    {
      if (pulseCount % 2 == 0)
      {
        int B = -lastPulse, V = B;
        signal.insert(signal.end(), {B, 0, 0, V});
        lastPulse = V;
      }
      else
      {
        int V = lastPulse;
        signal.insert(signal.end(), {0, 0, 0, V});
        lastPulse = V;
      }
      pulseCount = 0;
      i += 4;
    }
    else
    {
      if (data[i] == '1')
      {
        lastPulse = -lastPulse;
        signal.push_back(lastPulse);
        pulseCount++;
      }
      else
        signal.push_back(0);
      i++;
    }
  }
  return signal;
}

// ANALOG → DIGITAL

string pcm_encode(const vector<double> &analog_signal, int bits)
{
  string digital = "";
  double minVal = *min_element(analog_signal.begin(), analog_signal.end());
  double maxVal = *max_element(analog_signal.begin(), analog_signal.end());
  int levels = pow(2, bits);
  double step = (maxVal - minVal) / levels;

  for (double sample : analog_signal)
  {
    int quantized = min((int)((sample - minVal) / step), levels - 1);
    for (int i = bits - 1; i >= 0; i--)
      digital += ((quantized >> i) & 1) ? '1' : '0';
  }
  return digital;
}

string delta_modulation(const vector<double> &analog_signal, double delta)
{
  string digital = "";
  double approximation = analog_signal[0];
  for (size_t i = 1; i < analog_signal.size(); i++)
  {
    if (analog_signal[i] > approximation)
    {
      digital += '1';
      approximation += delta;
    }
    else
    {
      digital += '0';
      approximation -= delta;
    }
  }
  return digital;
}

// DISPLAY - ASCII Waveform

void displaySignal(const vector<int> &signal, const string &bitstream = "")
{
  cout << "\nEncoded Signal: ";
  for (int val : signal)
  {
    if (val == 1)
      cout << "+";
    else if (val == -1)
      cout << "-";
    else
      cout << "0";
  }
  cout << endl;

  cout << "\n=== ASCII Waveform Visualization ===\n";

  int samples_per_bit = 6;
  string line_pos1 = " +1 | ";
  string line_zero = "  0 | ";
  string line_neg1 = " -1 | ";
  string time_axis = "    +";
  string bit_labels = "Bit | ";

  for (size_t i = 0; i < signal.size(); ++i)
  {
    int level = signal[i];
    int prev_level = (i > 0) ? signal[i - 1] : 0;

    for (int j = 0; j < samples_per_bit; ++j)
    {
      if (j == 0 && i > 0)
      {
        if (prev_level == 1 && level == 0)
        {
          line_pos1 += '\\';
          line_zero += ' ';
          line_neg1 += ' ';
        }
        else if (prev_level == 1 && level == -1)
        {
          line_pos1 += '|';
          line_zero += '|';
          line_neg1 += ' ';
        }
        else if (prev_level == 0 && level == 1)
        {
          line_pos1 += ' ';
          line_zero += '/';
          line_neg1 += ' ';
        }
        else if (prev_level == 0 && level == -1)
        {
          line_pos1 += ' ';
          line_zero += '\\';
          line_neg1 += ' ';
        }
        else if (prev_level == -1 && level == 0)
        {
          line_pos1 += ' ';
          line_zero += ' ';
          line_neg1 += '/';
        }
        else if (prev_level == -1 && level == 1)
        {
          line_pos1 += ' ';
          line_zero += '|';
          line_neg1 += '|';
        }
        else
        {
          line_pos1 += (level == 1 ? '_' : ' ');
          line_zero += (level == 0 ? '_' : ' ');
          line_neg1 += (level == -1 ? '_' : ' ');
        }
      }
      else
      {
        line_pos1 += (level == 1 ? '_' : ' ');
        line_zero += (level == 0 ? '_' : ' ');
        line_neg1 += (level == -1 ? '_' : ' ');
      }

      time_axis += '-';
      if (j == samples_per_bit / 2)
        bit_labels += (!bitstream.empty() && i < bitstream.size()) ? bitstream[i] : ' ';
      else
        bit_labels += ' ';
    }

    line_pos1 += '|';
    line_zero += '|';
    line_neg1 += '|';
    time_axis += '+';
    bit_labels += ' ';
  }

  cout << line_pos1 << "\n"
       << line_zero << "\n"
       << line_neg1 << "\n"
       << time_axis << "\n"
       << bit_labels << "\n";

  cout << "\n--- Signal Statistics ---\n";
  int cp = count(signal.begin(), signal.end(), 1);
  int cz = count(signal.begin(), signal.end(), 0);
  int cn = count(signal.begin(), signal.end(), -1);
  int n = signal.size();

  cout << "Total bits: " << n << "\n";
  cout << "Positive (+1): " << cp << " (" << fixed << setprecision(1) << (cp * 100.0 / n) << "%)\n";
  cout << "Zero    (  0): " << cz << " (" << (cz * 100.0 / n) << "%)\n";
  cout << "Negative (-1): " << cn << " (" << (cn * 100.0 / n) << "%)\n";

  int violations = 0;
  for (size_t i = 1; i < signal.size(); ++i)
    if (signal[i] != 0 && signal[i] == signal[i - 1])
      violations++;
  cout << "Polarity violations: " << violations << "\n";
  cout << "===================================\n\n";
}

// GNUPLOT - PNG Waveform

void plotSignal(const vector<int> &signal, const string &title, const string &digitalData)
{
  ofstream dataFile("signal_data.txt");
  for (size_t i = 0; i < signal.size(); i++)
  {
    dataFile << i << " " << signal[i] << "\n";
    dataFile << (i + 1) << " " << signal[i] << "\n";
  }
  dataFile.close();

  ofstream scriptFile("plot_script.gnu");
  scriptFile << "set terminal png size 1200,600 enhanced font 'Arial,12'\n";
  scriptFile << "set output 'signal_plot.png'\n";
  scriptFile << "set title '" << title << " - Input: " << digitalData << "' font 'Arial,14'\n";
  scriptFile << "set xlabel 'Time (bit period)' font 'Arial,12'\n";
  scriptFile << "set ylabel 'Voltage Level' font 'Arial,12'\n";
  scriptFile << "set yrange [-1.5:1.5]\n";
  scriptFile << "set grid\n";
  scriptFile << "set style line 1 lc rgb '#0060ad' lt 1 lw 2\n";
  scriptFile << "plot 'signal_data.txt' with lines ls 1 notitle\n";
  scriptFile.close();

  int result = system("gnuplot plot_script.gnu");
  if (result == 0)
    cout << "\n✓ Graph generated: signal_plot.png\n";
  else
  {
    cout << "\n✗ Gnuplot not found. Install via:\n";
    cout << "  sudo apt-get install gnuplot  (Linux)\n";
    cout << "  brew install gnuplot          (Mac)\n";
  }
}

// DECODERS

string nrzl_decode(const vector<int> &signal)
{
  string data = "";
  for (int val : signal)
  {
    if (val == -1)
      data += '1';
    else if (val == 1)
      data += '0';
  }
  return data;
}

string nrzi_decode(const vector<int> &signal)
{
  string data = "";
  if (signal.empty())
    return data;
  int prev = signal[0];
  for (size_t i = 1; i < signal.size(); i++)
  {
    data += (signal[i] != prev) ? '1' : '0';
    prev = signal[i];
  }
  return data;
}

string manchester_decode(const vector<int> &signal)
{
  string data = "";
  for (size_t i = 0; i + 1 < signal.size(); i += 2)
  {
    if (signal[i] == -1 && signal[i + 1] == 1)
      data += '1';
    else if (signal[i] == 1 && signal[i + 1] == -1)
      data += '0';
  }
  return data;
}

string diff_manchester_decode(const vector<int> &signal)
{
  string data = "";
  for (size_t i = 2; i + 1 < signal.size(); i += 2)
    data += (signal[i - 1] != signal[i]) ? '0' : '1';
  return data;
}

string ami_decode(const vector<int> &signal)
{
  string data = "";
  for (int val : signal)
    data += (val == 0) ? '0' : '1';
  return data;
}

string b8zs_descramble(const vector<int> &signal)
{
  string data = "";
  for (size_t i = 0; i < signal.size();)
  {
    if (i + 7 < signal.size())
    {
      bool p1 = (signal[i] == 0 && signal[i + 1] == 0 && signal[i + 2] == 0 &&
                 signal[i + 3] == 1 && signal[i + 4] == -1 && signal[i + 5] == 0 &&
                 signal[i + 6] == -1 && signal[i + 7] == 1);
      bool p2 = (signal[i] == 0 && signal[i + 1] == 0 && signal[i + 2] == 0 &&
                 signal[i + 3] == -1 && signal[i + 4] == 1 && signal[i + 5] == 0 &&
                 signal[i + 6] == 1 && signal[i + 7] == -1);
      if (p1 || p2)
      {
        data += "00000000";
        i += 8;
        continue;
      }
    }
    data += (signal[i] == 0) ? '0' : '1';
    i++;
  }
  return data;
}

string hdb3_descramble(const vector<int> &signal)
{
  string data = "";
  for (size_t i = 0; i < signal.size();)
  {
    if (i + 3 < signal.size())
    {
      // B00V pattern
      if (signal[i] != 0 && signal[i + 1] == 0 && signal[i + 2] == 0 && signal[i + 3] != 0)
        if (signal[i] == signal[i + 3])
        {
          data += "0000";
          i += 4;
          continue;
        }

      // 000V pattern
      if (signal[i] == 0 && signal[i + 1] == 0 && signal[i + 2] == 0 && signal[i + 3] != 0)
      {
        bool isViolation = false;
        for (int j = (int)i - 1; j >= 0; j--)
          if (signal[j] != 0)
          {
            isViolation = (signal[j] == signal[i + 3]);
            break;
          }
        if (isViolation)
        {
          data += "0000";
          i += 4;
          continue;
        }
      }
    }
    data += (signal[i] == 0) ? '0' : '1';
    i++;
  }
  return data;
}

string decodeSignal(const vector<int> &signal, int encodingChoice, int scramblingType = 0)
{
  switch (encodingChoice)
  {
  case 1:
    return nrzl_decode(signal);
  case 2:
    return nrzi_decode(signal);
  case 3:
    return manchester_decode(signal);
  case 4:
    return diff_manchester_decode(signal);
  case 5:
    if (scramblingType == 1)
      return b8zs_descramble(signal);
    if (scramblingType == 2)
      return hdb3_descramble(signal);
    return ami_decode(signal);
  default:
    return "";
  }
}

// MAIN

int main()
{
  cout << "===== DIGITAL SIGNAL GENERATOR =====\n\n";

  int inputChoice;
  cout << "Select input type:\n";
  cout << "1. Digital input (for line encoding)\n";
  cout << "2. Analog input (for PCM/DM)\n";
  cout << "Enter choice (1 or 2): ";
  cin >> inputChoice;

  if (inputChoice != 1 && inputChoice != 2)
  {
    cout << "Invalid choice!\n";
    return 1;
  }

  string digitalData;
  vector<double> analogSignal;

  if (inputChoice == 2)
  {
    int modulationType;
    cout << "\nSelect modulation technique:\n";
    cout << "1. PCM (Pulse Code Modulation)\n";
    cout << "2. DM  (Delta Modulation)\n";
    cout << "Enter choice (1 or 2): ";
    cin >> modulationType;

    if (modulationType != 1 && modulationType != 2)
    {
      cout << "Invalid choice!\n";
      return 1;
    }

    int numSamples;
    cout << "Enter number of analog samples: ";
    cin >> numSamples;
    analogSignal.resize(numSamples);
    cout << "Enter " << numSamples << " analog values:\n";
    for (int i = 0; i < numSamples; i++)
    {
      cout << "Sample " << (i + 1) << ": ";
      cin >> analogSignal[i];
    }

    if (modulationType == 1)
    {
      int bits;
      cout << "Enter number of bits for quantization: ";
      cin >> bits;
      digitalData = pcm_encode(analogSignal, bits);
      cout << "\nPCM encoded: " << digitalData << "\n";
    }
    else
    {
      double delta;
      cout << "Enter delta value: ";
      cin >> delta;
      digitalData = delta_modulation(analogSignal, delta);
      cout << "\nDM encoded: " << digitalData << "\n";
    }

    cout << "\nProceeding to line encoding...\n";
  }
  else
  {
    cout << "Enter binary data stream: ";
    cin >> digitalData;
  }

  for (char c : digitalData)
    if (c != '0' && c != '1')
    {
      cout << "Error: Only 0s and 1s allowed.\n";
      return 1;
    }

  cout << "\n=== Line Encoding Schemes ===\n";
  cout << "1. NRZ-L\n2. NRZ-I\n3. Manchester\n";
  cout << "4. Differential Manchester\n5. AMI\n";
  cout << "Enter choice (1 to 5): ";

  int encodingChoice;
  cin >> encodingChoice;

  vector<int> encodedSignal;
  string encodingName;
  int scramblingTypeUsed = 0;

  switch (encodingChoice)
  {
  case 1:
    encodedSignal = nrzl_encode(digitalData);
    encodingName = "NRZ-L";
    break;
  case 2:
    encodedSignal = nrzi_encode(digitalData);
    encodingName = "NRZ-I";
    break;
  case 3:
    encodedSignal = manchester_encode(digitalData);
    encodingName = "Manchester";
    break;
  case 4:
    encodedSignal = diff_manchester_encode(digitalData);
    encodingName = "Differential Manchester";
    break;
  case 5:
  {
    char needScrambling;
    cout << "Do you need scrambling? (y/n): ";
    cin >> needScrambling;

    if (needScrambling == 'y' || needScrambling == 'Y')
    {
      int scramblingType;
      cout << "1. B8ZS\n2. HDB3\nEnter choice: ";
      cin >> scramblingType;
      if (scramblingType != 1 && scramblingType != 2)
      {
        cout << "Invalid choice!\n";
        return 1;
      }
      scramblingTypeUsed = scramblingType;
      if (scramblingType == 1)
      {
        encodedSignal = b8zs_scramble(digitalData);
        encodingName = "AMI with B8ZS";
      }
      else
      {
        encodedSignal = hdb3_scramble(digitalData);
        encodingName = "AMI with HDB3";
      }
    }
    else
    {
      encodedSignal = ami_encode(digitalData);
      encodingName = "AMI";
    }
    break;
  }
  default:
    cout << "Invalid choice!\n";
    return 1;
  }

  // Results
  cout << "\n===== RESULTS =====\n";
  cout << "Input Data:      " << digitalData << "\n";
  cout << "Encoding Scheme: " << encodingName << "\n";

  string palindrome = findLongestPalindrome(digitalData);
  cout << "Longest Palindrome: " << palindrome << " (Length: " << palindrome.length() << ")\n";

  displaySignal(encodedSignal, digitalData);
  plotSignal(encodedSignal, encodingName, digitalData);

  // Decode directly from encoded signal
  cout << "\n=== DECODING ===\n";
  char wantDecode;
  cout << "Decode the signal? (y/n): ";
  cin >> wantDecode;

  if (wantDecode == 'y' || wantDecode == 'Y')
  {
    string decodedData = decodeSignal(encodedSignal, encodingChoice, scramblingTypeUsed);

    cout << "\n===== DECODING RESULTS =====\n";
    cout << "Original: " << digitalData << "\n";
    cout << "Decoded:  " << decodedData << "\n";

    if (decodedData == digitalData)
    {
      cout << "\n✓ DECODING SUCCESSFUL! All " << digitalData.length() << " bits correct.\n";
    }
    else
    {
      int correct = 0;
      size_t minLen = min(digitalData.length(), decodedData.length());
      cout << "\nBit-by-bit comparison:\n";
      cout << "Pos | Orig | Decoded | Match\n";
      cout << "----|------|---------|------\n";
      for (size_t i = 0; i < minLen; i++)
      {
        bool match = (digitalData[i] == decodedData[i]);
        if (match)
          correct++;
        cout << setw(3) << i << " |  " << digitalData[i]
             << "   |    " << decodedData[i] << "    | " << (match ? "✓" : "✗") << "\n";
      }
      if (digitalData.length() != decodedData.length())
        cout << "Length mismatch: " << digitalData.length() << " vs " << decodedData.length() << "\n";
      cout << "Accuracy: " << fixed << setprecision(2)
           << (correct * 100.0 / digitalData.length()) << "%\n";
    }
  }

  cout << "\nProgram completed successfully!\n";
  return 0;
}
