# Diesel Engine Instrument PSD

This folder contains the final VGUS static-background PSD deliverable for the Huakuang diesel engine instrument screen.

Files:
- `柴油机仪表_VGUS静态底图.psd`: layered Photoshop-compatible PSD.
- `柴油机仪表_VGUS静态底图_preview.png`: preview image.
- `tachometer_pointer_fullcanvas.png`: exported full-canvas pointer layer reference.
- `generate_diesel_grayscale_vgus_static_psd.py`: generator script used to recreate the PSD.

Design notes:
- Dynamic numeric values are not baked into the PSD; add driven values in VGUS.
- Label, unit, parameter box, navigation buttons, pointer, and brand label are separated as logical layers.
- Navigation bar keeps 8 button slots, including blank reserved slots.
