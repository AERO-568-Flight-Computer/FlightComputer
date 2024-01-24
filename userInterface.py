import tkinter as tk
import os

def close_window():
    root.destroy()

def increase_value(var, step):
    current_value = float(var.get())
    new_value = round(current_value + step, 1)
    var.set(new_value)

def decrease_value(var, step):
    current_value = float(var.get())
    new_value = round(current_value - step, 1)
    var.set(new_value)

# Create the main window
root = tk.Tk()
root.title("Aircraft Control Interface")

# Set the window to full-screen
root.attributes('-fullscreen', True)

# Add a close button on the upper right with a red background and white font
close_button = tk.Button(root, text="✕", command=close_window, height=2, width=5, font=('Arial', 16), fg='black', bg='red', activebackground='red')
close_button.pack(side=tk.TOP, anchor=tk.NE, padx=10, pady=10)

# Add a title label at the top
title_label = tk.Label(root, text="Aircraft Control Interface", font=('Calibri', 24, 'bold'), fg='black')
title_label.pack(side=tk.TOP, fill=tk.X, pady=10)

# Servo Velocity setup
text_label_velocity = tk.Label(root, text="Servo Velocity:", font=('Calibri', 16), fg='black')
text_label_velocity.pack(side=tk.LEFT, anchor=tk.NW, padx=10, pady=10)

velocity_value = tk.DoubleVar()
velocity_value.set(0.0)
value_label_velocity = tk.Label(root, textvariable=velocity_value, font=('Calibri', 16), fg='black')
value_label_velocity.pack(side=tk.LEFT, anchor=tk.NE, padx=0, pady=10)

degree_symbol_label_velocity = tk.Label(root, text="°/sec", font=('Calibri', 16), fg='black')
degree_symbol_label_velocity.pack(side=tk.LEFT, anchor=tk.NW, padx=0, pady=10)

increase_button_velocity = tk.Button(root, text="+", command=lambda: increase_value(velocity_value, 0.1), font=('Arial', 14), fg='black')
increase_button_velocity.pack(side=tk.LEFT, anchor=tk.NE, padx=5, pady=10)

decrease_button_velocity = tk.Button(root, text="-", command=lambda: decrease_value(velocity_value, 0.1), font=('Arial', 14), fg='black')
decrease_button_velocity.pack(side=tk.LEFT, anchor=tk.NE, padx=5, pady=10)

# Servo Target Angle setup
text_label_angle = tk.Label(root, text="Servo Target Angle:", font=('Calibri', 16), fg='black')
text_label_angle.pack(side=tk.LEFT, anchor=tk.NW, padx=10, pady=10)

angle_value = tk.DoubleVar()
angle_value.set(0.0)
value_label_angle = tk.Label(root, textvariable=angle_value, font=('Calibri', 16), fg='black')
value_label_angle.pack(side=tk.LEFT, anchor=tk.NE, padx=0, pady=10)

degree_symbol_label_angle = tk.Label(root, text="°", font=('Calibri', 16), fg='black')
degree_symbol_label_angle.pack(side=tk.LEFT, anchor=tk.NW, padx=0, pady=10)

increase_button_angle = tk.Button(root, text="+", command=lambda: increase_value(angle_value, 1), font=('Arial', 14), fg='black')
increase_button_angle.pack(side=tk.LEFT, anchor=tk.NE, padx=5, pady=10)

decrease_button_angle = tk.Button(root, text="-", command=lambda: decrease_value(angle_value, 1), font=('Arial', 14), fg='black')
decrease_button_angle.pack(side=tk.LEFT, anchor=tk.NE, padx=5, pady=10)

# Start the main event loop
root.mainloop()
