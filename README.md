# PNG Stager

A PNG research project exploring custom chunk-based data embedding without modifying the visual content of the image.

PNG Stager allows binary data to be stored inside a PNG file by creating a custom PNG chunk while preserving the original image integrity.

---

## Overview

PNG files support custom chunks defined by the PNG specification.

This project demonstrates how a PNG image can be extended with an additional custom chunk containing arbitrary data while keeping the image fully viewable and valid.

The solution contains two Visual Studio C projects:

```
PNG-Stager
│
├── Payload-Staging
│   └── Creates a staged PNG with an embedded custom chunk
│
└── Loader
    └── Parses PNG chunks and retrieves embedded data and
```

---

## Features

- Create custom PNG chunks
- Embed binary data without modifying image pixels
- Preserve original PNG rendering
- Parse PNG chunk structure
- Extract custom chunk data

---

# Payload Staging

![Stager Help](/assets/img/stager-help.png)

The stager creates a new PNG chunk before the `IEND` chunk.

## Usage

```
Payload-Staging.exe [options]

-f, --file    <file>       Input PNG file (required)
-p, --payload <file>       Input binary file (required)
-o, --output  <file>       Output PNG file (default: staged.png)
-n, --name    <name>       Custom chunk name (default: dBIT)
-h, --help                 Show help message

Example: Payload-Staging.exe -f image.png -p payload.bin
```

![Staged PNG](/assets/img/stager.png)

---

# Loader

![Loader Help](/assets/img/loader-help.png)

The loader parses the PNG structure, searches for the selected chunk, and reads the embedded data.

Usage:

```bash
Usage: Loader.exe <png> <name>

Example: Loader.exe staged.png dBIT
```

![Loader PNG](/assets/img/loader.png)

---

## PNG Chunk Format

Every PNG chunk follows this structure:

```
+----------------+
| Length (4 byte)|
+----------------+
| Type   (4 byte)|
+----------------+
| Data           |
+----------------+
| CRC    (4 byte)|
+----------------+
```

The project creates a custom chunk:

```
Length  -> Embedded data size
Type    -> User-defined 4 byte identifier
Data    -> Embedded binary data
CRC     -> Calculated chunk checksum
```

---

# Roadmap

Future research and improvements:

- [ ] Embed data inside PNG pixel data
- [ ] Support additional image formats

---

# Author

**DarkBit**

X: https://x.com/DarkBitxx
Telegram: https://t.me/DarkBitx
