from get_share_path import get_share_path


def get_config():
    with open(get_share_path() + 'config.txt', 'r') as fobj:
        config = {}
        for line in fobj:
            key, value = line.split()
            config[key] = value
        return config
