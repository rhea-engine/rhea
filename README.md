## 💪 Developer Setup (Meworla Rhea Engine + Tools)

If you're working directly on the `Rhea` engine and its tools (like `Ceto`), follow these steps:

### 1. Install `mwpkg`

```bash
pip install mwpkg
```

### 2. Clone the main repository

```bash
git clone https://your.git.host/Meworla.git
cd Meworla
```

### 3. (Optional) Register the core engine package

If not already registered in `registry.json`:

```bash
mwpkg registry-add Meworla/Rhea --path .
```

### 4. Set the registry path

```bash
# PowerShell
$env:MWP_REGISTRY_PATH = "C:/your/path/to/Meworla/registry.json"

# or Bash
export MWP_REGISTRY_PATH=~/sources/Meworla/registry.json
```

You can add this to your shell profile to persist it.

### 5. Install dependencies for internal tools (e.g. Ceto)

```bash
cd Ceto
mwpkg install
```

This installs the modules listed in `Ceto/module.json` using the main repo's registry.

---

## 🎮 Game Developer Setup (Using Meworla Rhea Engine)

If you're making a game using the Rhea engine:

### 1. Create or open your game project

```bash
cd NewGame
```

### 2. Install the core engine

```bash
mwpkg install Meworla/Rhea
```

This installs the engine and its required modules.

### 3. (Optional) Add optional modules

```bash
mwpkg install Meworla/Lobby
```

These use the registry and will link automatically if your `MWP_REGISTRY_PATH` is set.

---

## 📁 Files Generated

* `module.json` — your declared dependencies
* `module-lock.json` — pinned versions for reproducibility
* `mwpkg.cmake` — included automatically to link module targets
