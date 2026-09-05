class Sys:

    """
    Objeto Sys
    ---
    controla los aspectos generales del programa
    """

    def __init__(self):
        self.print_separator = " "
        self.in_prompt = ""
        self.decimals_presicion = 6
        self.max_set_items = 100
        self.out_new_line = True
        self.sci_threshold = 1e6 

    def set(self, property, value):
        if property == "print_separator":
            self.print_separator = str(value)
        elif property == "in_prompt":
            self.in_prompt = str(value)
        elif property == "out_new_line":
            self.out_new_line = bool(value)
        elif property == "decimals_presicion":
            self.decimals_presicion = int(value)
        elif property == "max_set_items":
            self.max_set_items = int(value)
        else:
            raise ValueError(f"Propiedad Sys desconocida: {property}")

    def print(self, values : list):

        for val in values:
            print(val, end=self.print_separator)

        if self.out_new_line:
            print()

    def get_from_console(self):
        val = None
        while not val:
            val = input(self.in_prompt)
        return str(val)