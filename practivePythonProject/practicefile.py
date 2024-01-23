import matplotlib.pyplot as plt
import numpy as np

# Generate random data
np.random.seed(42)
x = np.linspace(0, 10, 100)
y = np.sin(x) + np.random.normal(0, 0.2, 100)

# Create a plot
plt.figure(figsize=(8, 6))
plt.plot(x, y, label='Random Example')

# Add labels and title
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.title('Random Example Plot')

# Add a legend
plt.legend()

# Display the plot
plt.show()


