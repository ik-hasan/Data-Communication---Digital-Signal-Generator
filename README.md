# Digital Signal Generator

A C++ based console application that encodes, visualizes, and decodes digital signals using various line encoding schemes. Also supports analog-to-digital conversion via PCM and Delta Modulation.

---

## Features

- **Line Encoding Schemes** – NRZ-L, NRZ-I, Manchester, Differential Manchester, AMI
- **Scrambling Techniques** – B8ZS and HDB3 (for AMI)
- **Analog-to-Digital Conversion** – PCM (Pulse Code Modulation) and Delta Modulation
- **ASCII Waveform Visualization** – Real-time terminal output
- **Gnuplot Graph Export** – Saves waveform as `signal_plot.png`
- **Signal Decoding** – Decodes encoded signal back to original bitstream with accuracy report
- **Longest Palindrome Detection** – Finds longest palindromic substring in the input bitstream using Manacher's Algorithm

---

## Requirements

| Tool | Purpose |
|------|---------|
| g++ (C++11 or later) | Compile the program |
| Gnuplot (optional) | Generate PNG waveform graphs |

### Install Gnuplot

```bash
# Linux
sudo apt-get install gnuplot

# macOS
brew install gnuplot
```

---

## Compilation

```bash
g++ -o signal_generator main.cpp -std=c++11
```

---

## Usage

```bash
./signal_generator
```

### Step 1 – Choose Input Type

```
1. Digital input (for line encoding)
2. Analog input (for PCM/DM)
```

### Step 2 – If Analog Input

- Choose **PCM** or **Delta Modulation**
- Enter number of samples and their values
- For PCM: specify number of quantization bits
- For DM: specify delta step value

### Step 3 – Choose Line Encoding Scheme

```
1. NRZ-L
2. NRZ-I
3. Manchester
4. Differential Manchester
5. AMI (with optional B8ZS or HDB3 scrambling)
```

### Step 4 – View Results

- Encoded signal is shown in terminal (ASCII waveform)
- Signal statistics are printed
- `signal_plot.png` is generated if Gnuplot is installed
- Option to decode the signal and verify accuracy

---

## Encoding Schemes

### NRZ-L (Non-Return to Zero Level)
- `1` → Negative voltage (`-1`)
- `0` → Positive voltage (`+1`)

### NRZ-I (Non-Return to Zero Inverted)
- `1` → Transition at beginning of bit
- `0` → No transition

### Manchester
- `1` → Low-to-High transition (`-1`, `+1`)
- `0` → High-to-Low transition (`+1`, `-1`)

### Differential Manchester
- `0` → Transition at start of bit period
- `1` → No transition at start of bit period
- Always has a transition in the middle

### AMI (Alternate Mark Inversion)
- `0` → Zero voltage
- `1` → Alternating `+1` / `-1`

### B8ZS Scrambling (for AMI)
Replaces 8 consecutive zeros with a special substitution pattern containing deliberate violations to maintain synchronization.

### HDB3 Scrambling (for AMI)
Replaces 4 consecutive zeros with a substitution pattern (B00V or 000V) based on polarity count.

---

## Analog-to-Digital Conversion

### PCM (Pulse Code Modulation)
Samples are quantized into `2^n` levels (where `n` = number of bits) and encoded as binary values.

### Delta Modulation (DM)
Tracks the analog signal with a step size `delta`:
- `1` if signal is rising
- `0` if signal is falling

---

## Output Files

| File | Description |
|------|-------------|
| `signal_plot.png` | Waveform graph (requires Gnuplot) |
| `signal_data.txt` | Raw signal data used for plotting |
| `plot_script.gnu` | Gnuplot script |

---

## Example

**Input:** `11001010`  
**Encoding:** Manchester

```
Encoded Signal: -+-++--+-+-++--+
```

Terminal waveform is drawn with `+1`, `0`, `-1` levels and transitions marked.

---

## Algorithms Used

- **Manacher's Algorithm** – O(n) longest palindrome substring detection on the input bitstream
- **Quantization** – Linear PCM quantization using min-max normalization
- **Polarity tracking** – Used in AMI, B8ZS, and HDB3 for violation detection

---

## Project Structure

```
.
├── main.cpp          # Full source code
├── signal_data.txt   # Generated during runtime
├── plot_script.gnu   # Generated during runtime
└── signal_plot.png   # Generated during runtime (if Gnuplot installed)
```

---

## License

This project is for educational purposes. Free to use and modify.
