// app/page.tsx
'use client';
import React from 'react';
import TitleScreen from "./TitleScreen";
import { redirect } from 'next/navigation';

export default function Page() { 
  return <TitleScreen onStart={() => redirect('/game')} />;
}
