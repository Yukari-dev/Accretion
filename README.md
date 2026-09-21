# Accretion
A real-time, GPU-native particle authoring tool. Users compose behaviors
from modules, scrub a timeline, and build everything from a campfire to
an accretion disk without leaving real-time.

## What is it
- A modular particle system built on Vulkan and C++.
- A timeline-based authoring tool.
- An optimizer that keeps hundreds of settings at 60fps

## What is it NOT
- Not a game engine.
- Not a Houdini competitor.
- Not a Shadertoy clone.
- Not a UI-first tool.

## Phases
1. **Kernel** — 1M particles, gravity + drag, points, playback.
2. **Modules** — 5 composable modules, generated UI.
3. **Optimizer** — 30 modules enabled, still 60fps.
4. **Timeline** — keyframes, events, scrub 1000 frames in <1s.

## Status
Phase 0 — setup. See DEVLOG.md for progress.
