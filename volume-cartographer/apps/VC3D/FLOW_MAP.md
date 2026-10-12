# VC3D User Flow Map

Every important user flow, its current state, and what was modernized.

## 1. Startup Flow
**Before:** Dead process → empty main window appears after volume load.
**After:**
- Welcome screen with recent projects, New/Open actions (animated fade-in).
- Main window shows *before* volume load — app appears immediately.
- Theme applied at startup.

## 2. Segmentation Flow (core loop)
**Before:** 8 panels in a scrolling list (Growth, ManualAdd, Editing,
ApprovalMask, DirectionField, NeuralTracer, Corrections, CustomParams).
Overwhelming, no guidance on order.
**After:** Grow / Refine / Advanced disclosure sections.
- Grow: expanded by default, primary action first.
- Refine: editing tools, expanded.
- Advanced: collapsed by default, smooth animated disclosure.
- All panel logic untouched; only grouping changed.

## 3. Workspace Layout (docks)
**Before:** 9 docks visible by default (Volumes, Segmentation, Seeding,
Viewer Controls, Composite, View, Normal Viz, Overlay, Segment Overlaps).
**After:** Progressive disclosure — 5 advanced docks hidden on first run.
All restorable from View menu. Saved layouts untouched.

## 4. 3D View Chrome
**Before:** Floating stats bar (unstyled), black void when no volume.
**After:**
- Stats bar styled as floating pill (theme).
- Empty state: "No volume loaded / Open a project to begin" centered
  overlay when no volume, auto-centered on resize.

## 5. Viewer Controls
**Status:** Already well-organized with collapsible groups (View,
Navigation, Normal Viz). No change needed.

## 6. Animations (Apple-like)
- CollapsibleSection: 280ms InOutCubic height animation.
- WelcomeDialog: 320ms OutCubic fade-in.
- Theme: pressed/hover states on all interactive elements.

## 7. Theme (global)
Dark Apple-like design system: docks, tabs, buttons, inputs, trees,
menus, scrollbars, section headers, viewer chrome. No widget logic touched.

## Out of Scope (blind)
- 3D renderer internals (shaders, lighting) — needs visual QA.
- SeedingWidget logic (2747 lines) — too risky without interactive test.
- Deep surface management workflows.
