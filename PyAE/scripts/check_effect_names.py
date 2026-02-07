import json

with open(r'C:\Users\ishikawa_yusuke\Documents\scene_dump.json', 'r', encoding='utf-8') as f:
    data = json.load(f)

items = data['items']
comp_items = [i for i in items if i.get('type') == 'Comp']
comp = comp_items[0]
layers = comp.get('comp_data', {}).get('layers', [])
layers_with_effects = [l for l in layers if l.get('effects')]

print(f'Comp: {comp.get("name")}')
for layer in layers_with_effects:
    print(f'\nLayer: {layer.get("name")}')
    effects = layer.get('effects', [])
    for effect in effects:
        print(f'  Effect name: {effect.get("name")}')
        print(f'  Effect match_name: {effect.get("match_name")}')
