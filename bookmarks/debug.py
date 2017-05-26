log_path = "./logs/dbg.sql"
#TODO implement help
#changing titles
#implement shell
#implement 3 types of table
#[optional] implement marks about watching/reading/listening

DBG_MODE_ON = 1

if DBG_MODE_ON:
    f = None
    def start_log():
        global f
        try:
            f = open(log_path, 'a')
        except:
            f = open(log_path, 'w')
    def log_print(*args):
        global f
        f.write(''.join(*args) + '\n')
    def end_log():
        global f
        f.close()
else:
    def start_log():
        pass
    def log_print(dummy):
        pass
    def end_log():
        pass         
