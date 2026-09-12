
tailwind.config = {
    theme: {
        extend: {
            fontFamily: {
                sans: ['Inter', 'sans-serif'],
                heading: ['Space Grotesk', 'sans-serif'],
                mono: ['JetBrains Mono', 'ui-monospace', 'monospace'],
            },
            colors: {
                heza: {
                    bg: '#03060a', /* Very dark, almost black blue */
                    surface: '#0a111a', /* Slightly lighter for cards */
                    cyan: '#00E5FF',
                    teal: '#0088AA',
                    textMuted: '#8b9bb4'
                }
            },
            backgroundImage: {
                'gradient-radial': 'radial-gradient(var(--tw-gradient-stops))',
            }
        }
    }
}