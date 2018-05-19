from get_config_path import get_config_path


def get_config():
    with open(get_config_path() + 'config.txt', 'r') as fobj:
        config = {}
        for line in fobj:
            key, value = line.split()
            config[key] = value
        return config


def set_config(config):
    with open(get_config_path() + 'config.txt', 'w') as fobj:
        for key, value in config.items():
            if key.startswith('_') or key.endswith('_'):
                continue
            if 'color' in key:
                r, g, b, _ = value.getRgb()
                fobj.write(
                    str(key) + ' ' + str(r) + ',' +
                    str(g) + ',' + str(b) + '\n')
                continue
            fobj.write(str(key) + ' ' + str(value) + '\n')
