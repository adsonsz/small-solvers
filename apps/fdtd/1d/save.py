import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Load data
data1 = np.loadtxt("ex.dat")
data2 = np.loadtxt("hy.dat")

Nt = min(data1.shape[0], data2.shape[0])
Nx = data1.shape[1]

x = np.linspace(0.0, 1.0, Nx)

# Create subplots
fig, (ax1, ax2) = plt.subplots(1, 2)

line1, = ax1.plot(x, data1[0])
line2, = ax2.plot(x, data2[0])

# Fix limits
ax1.set_xlim(x.min(), x.max())
ax2.set_xlim(x.min(), x.max())

ax1.set_ylim(data1.min(), data1.max())
ax2.set_ylim(data2.min(), data2.max())

ax1.set_title("Ex")
ax2.set_title("Hy")

# Animation
def update(frame):
    line1.set_ydata(data1[frame])
    line2.set_ydata(data2[frame])
    fig.suptitle(f"t = {frame}")
    return line1, line2

# Get animation
anim = FuncAnimation(fig, update, frames=Nt, interval=50, blit=True)

# Plot
plt.show()

# Save
anim.save("animation.mp4", fps=20)
# anim.save("animation.gif", fps=20)






