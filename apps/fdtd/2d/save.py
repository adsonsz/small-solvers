import numpy as np
import matplotlib.pyplot as plt
import os

# ===== Parameters =====
Nx, Ny = 256, 256      # grid size
L = 1.0              # domain extent
output_folder = "frames"

# ===== Prepare folder =====
os.makedirs(output_folder, exist_ok=True)

# ===== Load data =====
ez = np.loadtxt("ez.dat")
hx = np.loadtxt("hx.dat")
hy = np.loadtxt("hy.dat")

Nt = min(ez.shape[0], hx.shape[0], hy.shape[0])

# Global color limits (avoid flickering)
aa = 70.0
vmin = min(ez.min()/aa, hx.min()/aa, hy.min()/aa)
vmax = max(ez.max()/aa, hx.max()/aa, hy.max()/aa)

# ===== Create figure ONCE =====
fig, axes = plt.subplots(1, 3, figsize=(12, 4))

im1 = axes[0].imshow(
    np.zeros((Ny, Nx)), cmap='jet', origin='lower', extent=[-L, L, -L, L], vmin=vmin, vmax=vmax
)

im2 = axes[1].imshow(
    np.zeros((Ny, Nx)), cmap='jet', origin='lower', extent=[-L, L, -L, L], vmin=vmin, vmax=vmax
)

im3 = axes[2].imshow(
    np.zeros((Ny, Nx)), cmap='jet', origin='lower', extent=[-L, L, -L, L], vmin=vmin, vmax=vmax
)

# Titles
axes[0].set_title("Ez")
axes[1].set_title("Hx")
axes[2].set_title("Hy")

# Colorbars (one per plot)
plt.colorbar(im1, ax=axes[0])
plt.colorbar(im2, ax=axes[1])
plt.colorbar(im3, ax=axes[2])

# ===== Loop over time =====
for t in range(Nt):
    print(f"\rDoing frame {t}/{Nt}...", end='')
    # Verify if file already exists.
    filename = f"{output_folder}/{t:05d}.png"
    if os.path.isfile(filename): continue

    # Process
    grid_ez = ez[t].reshape((Ny, Nx))
    grid_hx = hx[t].reshape((Ny, Nx))
    grid_hy = hy[t].reshape((Ny, Nx))

    im1.set_data(grid_ez)
    im2.set_data(grid_hx)
    im3.set_data(grid_hy)

    fig.suptitle(f"t = {t}")
    plt.savefig(filename, dpi=100)

plt.close(fig)
print("Done saving frames.")

os.system("ffmpeg -framerate 30 -pattern_type glob -i 'frames/*.png' -c:v libx264 -pix_fmt yuv420p em30.mp4")
os.system("ffmpeg -framerate 60 -pattern_type glob -i 'frames/*.png' -c:v libx264 -pix_fmt yuv420p em60.mp4")
